#ifndef VPL_WALALA_H
#define VPL_WALALA_H

#include "opencv2/opencv.hpp"

namespace VPL {

class walala
{ 
  public:  
    walala();  
    ~walala();

    void TestPrint();
    void TestShowImage(const char *img_name);

  private:
};

}  // namespace VPL


#endif