/**********

Copyright 2017 Xilinx, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

**********/

#include "sqldate.h"
#include "sqldecimal.h"
#include "sqldecimalutils.h"
#include "sqlutils.h"

// SQL data format is 4 year like YYYY-MM-DD e.g. 2016-01-21
int SqlDate::toBin(char *dateStr, int curLoc) {
  year = dateStr[curLoc+3]-'0';
  year = 10*(dateStr[curLoc+2]-'0')+year;
  year = 100*(dateStr[curLoc+1]-'0')+year;
  year = 1000*(dateStr[curLoc]-'0')+year;

  month = dateStr[curLoc+6]-'0';
  month = 10*(dateStr[curLoc+5]-'0')+month;
    
  day = dateStr[curLoc+9]-'0';
  day = 10*(dateStr[curLoc+8]-'0')+day;
  curLoc = curLoc+11;
  return curLoc;
}

void SqlDate::print() {
  unsigned int yr = year;
  unsigned int mo = month;
  unsigned int dy = day;
  printf("%4d-%2d-%2d\n",yr,mo,dy);
}

void printStrLen(const char* ch, unsigned st, unsigned len) {
  for (unsigned i=st; i<len+st; i++) {
    std::cout << *ch ;
    ch++;
  }
  std::cout << std::endl;
}

void printStr(SqlDefs::RdDdrWord& rdWord) {
  for (unsigned int chIdx=0; chIdx < SqlDefs::RdDdrWordSize; chIdx++) {
    char ch = (rdWord>>(chIdx*8)) & 0x000FF;
    printf("%c",ch);
  }
  printf("\n");
}

int checkBlock(const char *bufRam, int puBlockSize, uint8_t puBlockCount, int totalFields, char fieldDelimiter, bool print) {
  uint8_t noOfPUBlocks = *((uint8_t *) bufRam);
  if (noOfPUBlocks != puBlockCount) {
    printf("Error: PU block counnt %d in block different than %d\n",noOfPUBlocks,puBlockCount);
  }
  int noOfRows = 0;
  const char *curBufRam = bufRam;
  for (int i=0; i<puBlockCount; i++) {
    noOfRows += checkPage(curBufRam, puBlockSize, totalFields, fieldDelimiter, false);
    curBufRam += puBlockSize;
  }
  if (print) printf("Info: Processed %d PUBlocks with %d rows\n",noOfPUBlocks, noOfRows);
  return noOfRows;
}

int checkPage(const char *bufRam, int blockSize, int totalFields, char fieldDelimiter, bool print) {
  //char noRows[4] = {0x0,bufRam[1],bufRam[2],bufRam[3]};
  unsigned int no_of_rows = (unsigned char) bufRam[3];
  no_of_rows += (unsigned char) bufRam[2] * (0x1<<8);
  no_of_rows += (unsigned char) bufRam[1] * (0x1<<16);
  bufRam = bufRam+4;
  if (print) printf("No of Rows = %d\n",no_of_rows);
  unsigned int cur_row = 0;
  int cur_buf_ptr = 0;
  int field_count = 0;
  while (cur_row < no_of_rows) {
    char cur_ch = bufRam[cur_buf_ptr++];
    if (cur_buf_ptr >= blockSize) {
      printf("Error: Corrupt DB page with mismatching no of rows\n");
      break;
    }
    if (print) printf("%c",cur_ch);
    if (cur_ch == fieldDelimiter)
      field_count++;
    if (field_count == totalFields) {
      field_count =0;
      cur_row++;
    }
  }
  if (cur_row != no_of_rows) {
    printf("Error: Corrupt DB page mismatching no of rows %d and %d\n",no_of_rows,cur_row);    
  }
  return no_of_rows;
}

int comparePages(const char *buf0, const char *buf1, int blockSize, int totalFields, char fieldDelimiter) {
  int rows0 = checkPage(buf0,blockSize,totalFields,fieldDelimiter,false);
  int rows1 = checkPage(buf1,blockSize,totalFields,fieldDelimiter,false);
  if (rows0 != rows1) {
    printf("Error: Pages with different rows %d and %d\n",rows0,rows1);
    return rows0;
  }
  int cur_row = 0;
  int cur_buf_ptr = 0;
  int field_count = 0;
  while (cur_row < rows0) {
    char ch0 = buf0[cur_buf_ptr];
    char ch1 = buf1[cur_buf_ptr++];    
    if (ch0 != ch1) {
      printf("Error: Pages with mismatching entries %c and %c\n", ch0, ch1);
      return rows0;
    }
    if (ch0 == '|') field_count++;
    if (field_count == totalFields) {
      field_count = 0;
      cur_row++;
    }
  }
  return rows0;
}
