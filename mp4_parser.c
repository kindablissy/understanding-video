#include <byteswap.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef struct {
  uint32_t size;
  char type[4];
} mp4_box_header;

typedef struct {
  uint32_t version;
} mp4_box_header_full_extension;

const unsigned long FULL_BOX_SIZE = sizeof(mp4_box_header) + sizeof(mp4_box_header_full_extension);

// first 4 bytes unsigned offset
// next 4  bytes = "ftype"
// next 4 brand version;
// next 4 compatible brands i.e. list of ascii text used technologies
typedef struct {
  mp4_box_header header;
  char major_brand[4];
  uint32_t m_brand_version;
  char *technologies_used;
} mp4_file_header;

typedef struct {
  mp4_box_header header;
  mp4_box_header_full_extension fullbox;
  uint64_t creation_time;
  uint64_t modification_time;
  uint32_t track_ID;
  uint32_t reserved;
  uint64_t duration;
  uint64_t __reserved;
  int16_t layer;
  int16_t alternate_group;
  int16_t volume;
  uint16_t ___reserved;
  int32_t matrix[9];
  uint32_t width;
  uint32_t height;
} trak_header_box;

typedef struct {
  mp4_box_header header;
} mdia_box;

typedef struct {
  uint32_t creation_time;
  uint32_t modification_time;
  uint32_t track_ID;
  uint32_t reserved;
  uint32_t duration;
} trak_header_box_v0;

typedef struct {
  mp4_box_header header;
  trak_header_box trak_header;

} trak_data_box;

const char MOVIE_DATA_HEADER[4] = {'m', 'o', 'o', 'v'};
const char TRAK_DATA_HEADER[4] = {'t', 'r', 'a', 'k'};
const char TRAK_HEADER_HEADER[4] = {'t', 'k', 'h', 'd'};
const char MEDIA_BOX_HEADER[4] = {'m', 'd', 'i', 'a'};
const char MEDIA_HEADER_HEADER[4] = {'m', 'd', 'h', 'd'};
const char MEDIA_INF_HEADER[4] = {'m', 'i', 'n', 'f'};
const char DATA_INF_HEADER[4] = {'d', 'i', 'n', 'f'};
const char SAMPLE_TABLE_HEADER[4] = {'s', 't', 'b', 'l'};

int read_mp4_box(FILE *mp4_file, mp4_box_header *header) {
  printf("\nreading box: \n");
  fread(header, sizeof(mp4_box_header), 1, mp4_file);
  header->size = bswap_32(header->size);
  printf("header_size:: %u\n", header->size);
  printf("header_name %s\n", header->type);
  return 0;
}


int read_mp4_full_box(FILE *mp4_file, mp4_box_header_full_extension *header) {
  printf("\nreading box: \n");
  fread(header, sizeof(mp4_box_header_full_extension), 1, mp4_file);
  printf("header_version:: %x\n", header->version);
  return 0;
}

int parse_mdia_box(FILE *mp4file, mp4_box_header *header) {
  return 0;
}

int parse_trak_header(FILE *mp4file, trak_header_box *trak_box) {
  read_mp4_full_box(mp4file, &trak_box->fullbox);
  int version = trak_box->fullbox.version << 24;
  printf("version::: %u\n", trak_box->fullbox.version << 24);
  if(version == 0) {
    trak_header_box_v0 v0box;
    fread(&v0box, sizeof(trak_header_box_v0), 1, mp4file);
    trak_box->track_ID = bswap_32(v0box.track_ID);
    trak_box->duration = bswap_32(v0box.duration);
    trak_box->reserved = bswap_32(v0box.reserved);
    trak_box->creation_time = bswap_32(v0box.creation_time);
    trak_box->modification_time = bswap_32(v0box.modification_time);
    printf("v0 duration::: %u\n", bswap_32(v0box.duration));
    printf("v0 reserved::: %u\n", bswap_32(v0box.reserved));
    printf("v0 trakid::: %u\n", bswap_32(v0box.track_ID));
    printf("v0 creation_time::: %u\n", bswap_32(v0box.creation_time));
  }else {
    
  }
  fread(&trak_box->__reserved,
        sizeof(uint64_t) + sizeof(int16_t) * 4 + sizeof(trak_box->matrix) + sizeof(trak_box->height) * 2,
        1, mp4file);
  printf("volume::: %u\n", bswap_16(trak_box->volume));
  printf("matrix::: %u\n", bswap_32(trak_box->matrix[4]));
  printf("width::: %u\n", bswap_32(trak_box->width << 16));
  printf("height::: %u\n", bswap_32(trak_box->height << 16));
  printf("current position::: %lu\n", ftell(mp4file));
  return 0;
}

int parse_trak(FILE *mp4file, trak_data_box *trak_data_box) {
  //parse_trak_header(mp4file, &trak_data_box->trak_header);
  return 0;
}

// assuming that the file has already been at the correct position;
int parse_moov(FILE *mp4file, mp4_box_header header) {
  // read all the block at once; (for now);
  // pass through for other boxes
  return 0;
}

/*
 * TODO:
 * This will be super slow ::::
 * Try to load a larger chunk of data at once and then do the parsing stuff;
 */
int parse_mp4(const char *filename) {
  FILE *mp4file = fopen(filename, "rb");
  unsigned long file_size = 0;

  fseek(mp4file, 0L, SEEK_END);
  file_size = ftell(mp4file);
  fseek(mp4file, 0L, 0);

  printf("filesize %ld\n", file_size);
  if (file_size <= 0) {
    printf("emptyfile received");
    return -1;
  }

  while (ftell(mp4file) < file_size) {
    mp4_box_header header;
    unsigned long extension = 0;
    read_mp4_box(mp4file, &header);
    printf("databox size: %u\n", header.size);
    printf("databox name:: %s\n", header.type); /* mdat box */

    if (memcmp(header.type, MOVIE_DATA_HEADER, 4) == 0) {
      parse_moov(mp4file, header);
    }
    else if (memcmp(header.type, TRAK_DATA_HEADER, 4) == 0) {
      trak_data_box trak_data_box;
      parse_trak(mp4file, &trak_data_box);
    }
    else if (memcmp(header.type, TRAK_HEADER_HEADER, 4) == 0) {
      trak_header_box header_box;
      header_box.header = header;
      parse_trak_header(mp4file, &header_box);
      extension = sizeof(mp4_box_header_full_extension);
    }
    else if (memcmp(header.type, MEDIA_BOX_HEADER, 4) == 0) {
    }
    else if (memcmp(header.type, MEDIA_INF_HEADER, 4) == 0) {
    }
    else if (memcmp(header.type, DATA_INF_HEADER, 4) == 0) {
    }
    else if (memcmp(header.type, SAMPLE_TABLE_HEADER, 4) == 0) {
    }
    else {
      fseek(mp4file, header.size - sizeof(header) - extension, SEEK_CUR);
    }
    printf("current file position:: %ld\n", ftell(mp4file));
  }

  printf("final file position:: %ld\n", ftell(mp4file));

  /*
  printf("offset:              %u\n", __bswap_constant_32(header.header.size));
  printf("ftype:               %c%c%c%c\n", header.ftypestring[0],
         header.ftypestring[1], header.ftypestring[2], header.ftypestring[3]);
  printf("major brand:         %c%c%c%c\n", header.major_brand[0],
         header.major_brand[1], header.major_brand[2], header.major_brand[3]);
  printf("major brand version: %u\n",
  __bswap_constant_32(header.m_brand_version));

  printf("current file position:: %ld\n", ftell(mp4file));
  header.technologies_used = (char *)malloc(header.offset - ftell(mp4file));
  fread(header.technologies_used, sizeof(char) * 16, 1, mp4file);
  printf("current file position:: %ld\n", ftell(mp4file));
  printf("technologies used string: %s\n", header.technologies_used);

  // Time to read some data
  uint32_t box_size;
  char box_name[4] 
  printf("current file position:: %ld\n", ftell(mp4file));

  fread(&box_size, sizeof(box_size), 1, mp4file);
  printf("databox size: %u\n", __bswap_constant_32(box_size));
  fread(box_name, sizeof(box_name), 1, mp4file);
  printf("databox name:: %s\n", box_name);
  */

  /*
   * TODO::: do something abstract
   * just doing this because of the sample file i'm using and the previous box
   * was "free" box
   */

  fclose(mp4file);
  return 0;
}

int parse_mdat(char *buffer) {
  uint32_t box_size = __bswap_constant_32(((uint32_t *)buffer)[0]);
  unsigned long offset = 8;
  return 0;
}

int parse_nal_unit(char *buffer) { return 0; }

int main() {
  parse_mp4("./1718848243_Sample_2.mp4");
  return 0;
}
