#pragma once

#include <iostream>
#include <fstream>
#include <vector>

class http_CD_{

public:
    http_CD_();
    ~http_CD_();
    void setCDType_();
    void setName_();
    void setFilename_();
    void setInnerContentType_();
    void setContent_();
    void setFileContent_();

    static int getGlobalIndex_();
    const int getInstanceIndex_() const;
    const std::string getCDType_() const;
    const int getName_() const;
    const std::string getFilename_() const;
    const std::string getInnerContentType_() const;
    const std::string getContent_() const;
    const std::string getFileContent_() const;
    const std::string getCDType_() const;

private:
    static int global_index_;
    int instance_index_;
    std::string CD_type_;
    std::string name_;
    std::string filename_;
    std::string inner_content_type_;
    std::string content_;
    std::string file_content_;
};

enum http_CD_header{
    ContentDisposition,
    InterContentType,
    Cont,
    FileContent
};


class http_content{

public:
    http_content();
    ~http_content();
    void setMethod();
    void setPath();
    void setVersion();
    void setFormat();
    void setBoundary();
    void setOpenBoundary();
    void setCloseBoundary();
    void setBodyWithNoCD();



    static int getGlobalIndex();
    const int getInstanceIndex() const;
    const std::string getMethod() const;
    const int getPath() const;
    const std::string getVersion() const;
    const std::string getFormat() const;
    const std::string getBoundary() const;
    const std::string openBoundary() const;
    const std::string closeBoundary() const;
    const std::string getBodyWithNoCD() const;
    std::string getCDs_list();


private:
    static int global_index_;
    int instance_index_;
    std::string path_;
    std::string version_;
    std::string format_;
    std::string boundary_;
    std::string open_boundary_;
    std::string close_boundary_;
    std::string body_with_no_cd_;
    std::vector<http_CD_> CDs_list_;

};