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
  uint32_t size;
  char type[4];
  uint32_t version;
} mp4_box_header_full;

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
  mp4_box_header_full header;
  uint64_t creation_time;
  uint64_t modification_time;
  uint32_t track_ID;
  uint32_t reserved;
  int16_t layer;
  int16_t alternate_group;
  int16_t volume;
} trak_header_box;

typedef struct {
  mp4_box_header header;
  trak_header_box trak_header;

} trak_data_box;

const char MOVIE_DATA_HEADER[4] = {'m', 'o', 'o', 'v'};
const char TRAK_DATA_HEADER[4] = {'t', 'r', 'a', 'k'};

int read_mp4_box(FILE *mp4_file, mp4_box_header *header) {
  printf("\nreading box: \n");
  fread(header, sizeof(mp4_box_header), 1, mp4_file);
  header->size = bswap_32(header->size);
  printf("header_size:: %u\n", header->size);
  printf("header_name %s\n", header->type);
  return 0;
}

int read_mp4_full_box(FILE *mp4_file, mp4_box_header_full *header) {
  printf("\nreading box: \n");
  fread(header, sizeof(mp4_box_header_full), 1, mp4_file);
  header->size = bswap_32(header->size);
  header->version = bswap_32(header->version);
  printf("header_size:: %u\n", header->size);
  printf("header_name %s\n", header->type);
  return 0;
}

int parse_trak_header(FILE *mp4file, trak_header_box *trak_box) {
  read_mp4_full_box(mp4file, &trak_box->header);
  printf("version::: %u", trak_box->header.version);
  fseek(mp4file, trak_box->header.size - sizeof(mp4_box_header_full), SEEK_CUR);
  return 0;
}

int parse_trak(FILE *mp4file, trak_data_box *trak_data_box) {
  parse_trak_header(mp4file, &trak_data_box->trak_header);
  return 0;
}

// assuming that the file has already been at the correct position;
int parse_moov(FILE *mp4file, mp4_box_header header) {
  // read all the block at once; (for now);
  unsigned long current_position = ftell(mp4file);
  printf("reading moov %ld", current_position);
  while (ftell(mp4file) < current_position + header.size - 8) {
    mp4_box_header smaller_box_header;
    read_mp4_box(mp4file, &smaller_box_header);
    if(memcmp(smaller_box_header.type, TRAK_DATA_HEADER, 4)) {
      trak_data_box thb;
      parse_trak(mp4file, &thb);
    }else {
      fseek(mp4file, smaller_box_header.size - sizeof(smaller_box_header),
            SEEK_CUR);
    }
    printf("[moov]:: current file position:: %ld\n", ftell(mp4file));
  }
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
    read_mp4_box(mp4file, &header);
    printf("databox size: %u\n", header.size);
    printf("databox name:: %s\n", header.type); /* mdat box */

    if (memcmp(header.type, MOVIE_DATA_HEADER, 4) == 0) {
      parse_moov(mp4file, header);
    } else {
      fseek(mp4file, header.size - sizeof(header), SEEK_CUR);
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
  char box_name[4];
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
