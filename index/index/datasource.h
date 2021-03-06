//
//  data.h
//  index
//
//  Created by Hanzhou Li on 3/2/13.
//  Copyright (c) 2013 Hanzhou Li. All rights reserved.
//

#ifndef __index__data__
#define __index__data__

#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include "Utility.h"
#include "GlobalConfig.h"

typedef struct {
    std::string url; // max url lenth in IE
    int depth;
    int unknown;
    int length;
    char IP[18];
    int port;
    char status[8];
    
} IndexRecord;

typedef std::vector<IndexRecord *> IndexRecordVector;

/****************************************************
 Return a vector containing the data files' names.
 This method will get all the files in the path recursively.
 @path: The path of the folder containing the files.
 ***************************************************/
StringVector *GetDataFileList(const char *path);


/****************************************************
 Return a vector of the index file of the data set.
 @indexBuf: The content of the index file.
 ****************************************************/
IndexRecordVector *ParseIndex(char *indexBuf);

/****************************************************
 Return a vector of the pages' content parsed from 
 the data file.
 @dataBuf: The content of the data file to be parsed.
 @indexArray: The vector array contain the responding
 indice of the data file.
 ****************************************************/
CStringVector *GetPages(char* dataBuf, IndexRecordVector* indexArray);


#endif /* defined(__index__data__) */
