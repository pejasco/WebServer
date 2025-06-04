#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>


//this 
class http_CD_{

public:
    http_CD_();
    ~http_CD_();
    //setter
    void setCDType_(std::string type);
    void setName_(std::string name);
    void setFilename_(std::string filename);
    void setInnerContentType_(std::string type);
    void setContent_(std::string content);
    void setFileContent_(std::string file_content);
    //getter
    static int getGlobalIndex_();
    const int getInstanceIndex_() const;
    const std::string getCDType_() const;
    const std::string getName_() const;
    const std::string getFilename_() const;
    const std::string getInnerContentType_() const;
    const std::string getContent_() const;
    const std::string getFileContent_() const;
    void printHttpCD();


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

//this is like the content
class http_content{

public:
    http_content();
    ~http_content();
    //setter
    void setBoundary(std::string boundary);
    void setOpenBoundary(std::string boundary);
    void setCloseBoundary(std::string boundary);
    void setBodyWithNoCD(std::string body);
    void setHttpCD(std::string body);
    //getter
    static int getGlobalIndex();
    const int getInstanceIndex() const;
    const std::string getBoundary() const;
    const std::string getOpenBoundary() const;
    const std::string getCloseBoundary() const;
    const std::string getBodyWithNoCD() const;
    std::vector<http_CD_>& getCDs_list_();
    void printHttpContent();

private:
    static int global_index_;
    int instance_index_;
    // std::string method_;
    // std::string path_;
    // std::string format_;
    std::string boundary_;
    std::string open_boundary_;
    std::string close_boundary_;
    std::string body_with_no_cd_;
    std::vector<http_CD_> CDs_list_;

};

//this is like the RequestHeader;
class http {

public:
    http();
    ~http();
    //setter
    void setMethod(std::string method);
    void setPath(std::string path);
    void setFormat(std::string format);
    void setHost(std::string host);
    void setContentType(std::string type);
    void setContent(std::string content);
    //getter
    const std::string getMethod() const;
    const std::string getPath() const;
    const std::string getFormat() const;
    const std::string getHost() const;
    const std::map<std::string, std::string> getContentType() const;
    http_content& getContent();
    
    void setHttp(std::string line_input);
    void printHttp();  



private:
    static int global_index_;
    int instance_index_;
    std::string method_;
    std::string path_;
    std::string format_;
    std::string host_;
    std::string boundary_;
    std::map<std::string, std::string> content_type_;
    http_content content_;
    
};