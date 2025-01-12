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

int parse_mp4(const char *filename) {
  FILE *mp4file = fopen(filename, "rb");

  mp4_file_header header;

  fread(&header, sizeof(header), 1, mp4file);

  printf("offset:              %u\n", __bswap_constant_32(header.offset));
  printf("ftype:               %c%c%c%c\n", header.ftypestring[0],
         header.ftypestring[1], header.ftypestring[2], header.ftypestring[3]);
  printf("major brand:         %c%c%c%c\n", header.major_brand[0],
         header.major_brand[1], header.major_brand[2], header.major_brand[3]);
  printf("major brand version: %u\n", __bswap_constant_32(header.m_brand_version));

  header.technologies_used = (char *)malloc(header.offset - 16);
  fread(header.technologies_used, 8, header.offset - 16, mp4file);
  printf("technologies used string: %s\n", header.technologies_used);

  fclose(mp4file);
  return 0;
}

int main() {
  parse_mp4("./1718848243_Sample_2.mp4");
  return 0;
}
