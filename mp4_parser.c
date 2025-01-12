#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <byteswap.h>
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// first 4 bytes unsigned offset
// next 4  bytes = "ftype"
// next 4 brand version;
// next 4 compatible brands i.e. list of ascii text used technologies
typedef struct mp4_file_header {
  uint32_t offset;
  char ftypestring[4];
  char major_brand[4];
  uint32_t m_brand_version;
  char *technologies_used;
} mp4_file_header;


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

  mp4_file_header header;

  printf("current file position:: %ld\n", ftell(mp4file));
  fread(&header, sizeof(header) - sizeof(header.technologies_used), 1, mp4file);

  printf("offset:              %u\n", __bswap_constant_32(header.offset));
  printf("ftype:               %c%c%c%c\n", header.ftypestring[0],
         header.ftypestring[1], header.ftypestring[2], header.ftypestring[3]);
  printf("major brand:         %c%c%c%c\n", header.major_brand[0],
         header.major_brand[1], header.major_brand[2], header.major_brand[3]);
  printf("major brand version: %u\n", __bswap_constant_32(header.m_brand_version));

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
  printf("databox name:: %s\n", box_name); /* free */



  /* 
   * TODO::: do something abstract
   * just doing this because of the sample file i'm using and the previous box was "free" box
  */

  printf("reading another block: \n");
  fread(&box_size, sizeof(box_size), 1, mp4file);
  box_size = __bswap_constant_32(box_size);
  printf("databox size: %u\n", box_size);
  fread(box_name, sizeof(box_name), 1, mp4file);
  printf("databox name:: %s\n", box_name); /* mdat box */

  printf("reading another block: \n");
  char *data = (char *) malloc(box_size - ftell(mp4file));
  if (!data) {
    printf("no mem??");
    return 1;
  }
  fread(data, (box_size - 8), 1, mp4file);
  printf("\n\nreading main data block: \n");
  /* since the file didnt contain chars "wide" */
  box_size = __bswap_constant_32(((uint32_t *) data)[0]);
  printf("databox size: %u\n", box_size);


  printf("current file position:: %ld\n", ftell(mp4file));

  fclose(mp4file);
  return 0;
}

int parse_mdat(char *buffer) {
  uint32_t box_size = __bswap_constant_32(((uint32_t *) buffer)[0]);
  unsigned long offset = 8;

  
  return 0;
}

int parse_nal_unit(char *buffer) {
  return 0;
}

int main() {
  parse_mp4("./1718848243_Sample_2.mp4");
  return 0;
}
