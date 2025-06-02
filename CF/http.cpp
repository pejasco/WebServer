#include "http.hpp"
#include <iostream>


int http_CD_::global_index_ = 0;
int http_content = 0;


http_CD_::http_CD_() : instance_index_(global_index_++) {
	// Constructor body
	std::cout << "http_CD_ instance created with index: " << instance_index_ << std::endl;
}

http_CD_::~http_CD_(){}

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










int http_CD_::global_index_ = 0;
int http_content = 0;


http_CD_::http_CD_() : instance_index_(global_index_++) {
	// Constructor body
	std::cout << "http_CD_ instance created with index: " << instance_index_ << std::endl;
}




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
