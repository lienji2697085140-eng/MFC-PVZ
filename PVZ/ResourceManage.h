#pragma once
#include <afxwin.h>
#include <atlimage.h>
#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <sstream>
using std::filesystem::current_path;
using std::filesystem::exists;
using std::filesystem::directory_iterator;
using std::filesystem::directory_entry;
using std::filesystem::file_type;
using std::filesystem::file_status;
using std::filesystem::path;

class ResourceManage;

struct ImageRes {

    friend class ResourceManage;

public:
    ImageRes(const std::string& className, size_t s, size_t a, const std::string& path) :
            theClassName(className), state(s), animateTick(a), filePath(path), img(new CImage) {
        img->Load(this->filePath.c_str());
        
        // 对阳光特殊处理
        if (className == "Sun") {
            for (int i = 0; i < img->GetWidth(); ++i) {
                for (int j = 0; j < img->GetHeight(); ++j) {
                    byte* pByte = (byte*)img->GetPixelAddress(i, j);
                    pByte[0] = pByte[0] * pByte[3] / 255;
                    pByte[1] = pByte[1] * pByte[3] / 255;
                    pByte[2] = pByte[2] * pByte[3] / 255; 
                }
            }
        }

        //if (className == "NormalZombie") {
        //    for (int i = 0; i < img->GetWidth(); ++i) {
        //        for (int j = 0; j < img->GetHeight(); ++j) {
        //            byte* pByte = (byte*)img->GetPixelAddress(i, j);
        //            pByte[0] = 200;// pByte[1] * pByte[3] / 100;
        //            // pByte[1] = 0;//pByte[1] * pByte[3] / 255;
        //            // pByte[2] = 0;//pByte[2] * pByte[3] / 255;
        //        }
        //    }
        //}
    }

    bool operator<(const ImageRes& img) const {
        return animateTick < img.animateTick;
    }

    CImage* operator->() {
        return img.get();
    }

    bool changeColor(CImage* destImage) {
        if (img->IsNull())
            return FALSE;
        //源图像参数
        BYTE* srcPtr = (BYTE*)img->GetBits();
        int srcBitsCount = img->GetBPP();
        int srcWidth = img->GetWidth();
        int srcHeight = img->GetHeight();
        int srcPitch = img->GetPitch();
        //销毁原有图像
        if (!destImage->IsNull())
        {
            //destImage->Destroy();
        }
        //创建新图像
        //if (srcBitsCount == 32)   //支持alpha通道
        //{
            destImage->Create(srcWidth, srcHeight, srcBitsCount, 1);
        /*}
        else
        {
            destImage->Create(srcWidth, srcHeight, srcBitsCount, 0);
        }*/
        //加载调色板
        if (srcBitsCount <= 8 && img->IsIndexed())//需要调色板
        {
            RGBQUAD pal[256];
            int nColors = img->GetMaxColorTableEntries();
            if (nColors > 0)
            {
                img->GetColorTable(0, nColors, pal);
                destImage->SetColorTable(0, nColors, pal);//复制调色板程序
            }
        }
        //目标图像参数
        BYTE* destPtr = (BYTE*)img->GetBits();
        int destPitch = destImage->GetPitch();
        //复制图像数据
        for (int i = 0; i < srcHeight; i++)
        {
            memcpy(destPtr + i * destPitch, srcPtr + i * srcPitch, abs(srcPitch));
        }

        return true;
    }

public:
    std::string theClassName;
    std::string filePath;
    size_t state = 0;
    size_t animateTick = 0;
    std::unique_ptr<CImage> img;
};

// 定义资源文件名格式
// 类名_资源枚举ID
class ResourceManage {

public:
    using class_image = std::vector<ImageRes>;
    using class_all_image = std::map<size_t, class_image>;
    using image_res_type = std::map<std::string, class_all_image>;
    
    
    // using res_type = std::map<std::string, std::map<int, std::vector<音乐资源?>>>

public:
    ResourceManage(const std::string& path) : images() {
        auto currDir = std::filesystem::current_path();
        currDir.append(path);
        if (!exists(currDir)) throw std::runtime_error("path error");
        directory_iterator list(currDir);
        for (const auto& item : list) {
            if (item.status().type() == file_type::directory) {
                // 加载图片
                if (item.path().filename() == "image")
                    loadImages(directory_iterator{ item.path() });
            }
        }
        // 排序图片
        orderImages();
    }

    void loadImages(const directory_iterator& list) {
        // 判断文件夹是否为空
        if (list->path().empty()) {
            return;
        }
        for (const auto& item : list) {
            if (item.status().type() == file_type::directory) {
                loadImages(directory_iterator{ item.path() });
            }
            else {
                iterationImages(item.path());
            }
        }
    }

    void iterationImages(const path& imgPath) {
        std::string fileName = imgPath.stem().string();
        // std::cout << fileName << std::endl;
        std::stringstream sstrm(fileName);
        std::string className;
        size_t state, animateTick;
        sstrm >> className >> state >> animateTick;
        // 构造Img对象
        images[className][state].emplace_back(className, state, animateTick, imgPath.string());
    }

    void orderImages() {
        for (auto& item : images) {
            for (auto& map : item.second) {
                auto& vec = map.second;
                std::sort(vec.begin(), vec.end(), std::less<ImageRes>());
            }
        }
    }

public:
    class_image& getResource(const std::string& theClassName, size_t state) {
        return images.at(theClassName).at(state);
    }

    class_all_image& getRes(const std::string& theClassName) {
        return images.at(theClassName);
    }

private:
    // 所有资源序列
    image_res_type images;
};

