//
//  urltable.cpp
//  index
//
//  Created by Hanzhou Li on 3/2/13.
//  Copyright (c) 2013 Hanzhou Li. All rights reserved.
//

#include "urltable.h"

URLTable::URLTable()
{
    _urlTable = new URLVector;
}

URLTable::~URLTable()
{
    for(int i=0;i<_urlTable->size();i++) {
        delete (*_urlTable)[i];
        (*_urlTable)[i] = NULL;
    }
    _urlTable->clear();
    delete _urlTable;
    _urlTable = NULL;
}

void URLTable::Add(URLItem* urlitem)
{
    _urlTable->push_back(urlitem);
}

void URLTable::Write(FILEMODE mode)
{
    if(mode == FILEMODE_ASCII) {
        FILE *fp = fopen(URL_FILE, "w+");
        for(int i=0;i<_urlTable->size();i++) {
            URLItem *urlitem =(*_urlTable)[i];
            fprintf(fp,"%s %d %d\n", urlitem->url.data(), urlitem->fileID, urlitem->startIndex);
        }
        
        fclose(fp);
    }
    
    if(mode == FILEMODE_BIN) {
        FILE *fp = fopen(URL_FILE, "wb+");
        for(int i=0;i<_urlTable->size();i++) {
            URLItem *urlitem =(*_urlTable)[i];
            uint16_t urlLength = urlitem->url.length();
            fwrite(&urlLength, sizeof(uint16_t),1, fp);
            fwrite(urlitem->url.data(), sizeof(char), urlLength, fp);
            fwrite(&urlitem->fileID, sizeof(uint16_t), 1, fp);
            fwrite(&urlitem->startIndex, sizeof(uint32_t), 1, fp);
        }
        fclose(fp);
    }
}
