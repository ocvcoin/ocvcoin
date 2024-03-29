package=opencv
$(package)_version=70bbf17b133496bd7d54d034b0f94bd869e0e810
$(package)_download_path=https://github.com/opencv/opencv/archive
$(package)_file_name=$($(package)_version).tar.gz
$(package)_sha256_hash=74e2250fcac2a51996be85f46e2aac386e31b4faaf6a9aa6bae94d0fbb4102e7
$(package)_build_subdir=build

define $(package)_preprocess_cmds
  mkdir build && sed -i '21,22 s/^/#/' cmake/OpenCVCompilerOptions.cmake && sed -i 's/PyString_AsString(obj);/(char*)PyString_AsString(obj);/' modules/python/src2/cv2.cpp && sed -i 's/add_extra_compiler_option(-Werror=address)//' cmake/OpenCVCompilerOptions.cmake
endef



define $(package)_config_cmds
  $($(package)_cmake) -DCMAKE_SKIP_RPATH="ON" -DCMAKE_EXE_LINKER_FLAGS="-static" -DCMAKE_FIND_LIBRARY_SUFFIXES=".a" -DBUILD_opencv_flann=OFF -DBUILD_opencv_ml=OFF -DBUILD_opencv_video=OFF -DBUILD_opencv_shape=OFF -DBUILD_opencv_videoio=OFF -DBUILD_opencv_highgui=OFF -DBUILD_opencv_objdetect=OFF -DBUILD_opencv_superres=OFF -DBUILD_opencv_ts=OFF -DBUILD_opencv_features2d=OFF -DBUILD_opencv_calib3d=OFF -DBUILD_opencv_stitching=OFF -DBUILD_opencv_videostab=OFF -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_LIBS=ON -DBUILD_ZLIB=OFF -DBUILD_TIFF=OFF -DBUILD_JASPER=OFF -DBUILD_JPEG=OFF -DBUILD_PNG=OFF -DBUILD_OPENEXR=OFF -DBUILD_TBB=OFF -DBUILD_WITH_STATIC_CRT=ON -DINSTALL_C_EXAMPLES=OFF -DINSTALL_PYTHON_EXAMPLES=OFF -DWITH_1394=OFF -DWITH_AVFOUNDATION=OFF -DWITH_CARBON=OFF -DWITH_CAROTENE=OFF -DWITH_VTK=OFF -DWITH_CUDA=OFF -DWITH_CUFFT=OFF -DWITH_CUBLAS=OFF -DWITH_NVCUVID=OFF -DWITH_EIGEN=OFF -DWITH_VFW=OFF -DWITH_FFMPEG=OFF -DWITH_GSTREAMER=OFF -DWITH_GSTREAMER_0_10=OFF -DWITH_GTK=OFF -DWITH_GTK_2_X=OFF -DWITH_IPP=OFF -DWITH_JASPER=OFF -DWITH_JPEG=OFF -DWITH_WEBP=OFF -DWITH_OPENEXR=OFF -DWITH_OPENGL=OFF -DWITH_OPENVX=OFF -DWITH_OPENNI=OFF -DWITH_OPENNI2=OFF -DWITH_PNG=OFF -DWITH_GDCM=OFF -DWITH_PVAPI=OFF -DWITH_GIGEAPI=OFF -DWITH_ARAVIS=OFF -DWITH_QT=OFF -DWITH_WIN32UI=OFF -DWITH_QUICKTIME=OFF -DWITH_QTKIT=OFF -DWITH_TBB=OFF -DWITH_OPENMP=OFF -DWITH_CSTRIPES=OFF -DWITH_PTHREADS_PF=OFF -DWITH_TIFF=OFF -DWITH_UNICAP=OFF -DWITH_V4L=OFF -DWITH_LIBV4L=OFF -DWITH_DSHOW=OFF -DWITH_MSMF=OFF -DWITH_XIMEA=OFF -DWITH_XINE=OFF -DWITH_CLP=OFF -DWITH_OPENCL=OFF -DWITH_OPENCL_SVM=OFF -DWITH_OPENCLAMDFFT=OFF -DWITH_OPENCLAMDBLAS=OFF -DWITH_DIRECTX=OFF -DWITH_INTELPERC=OFF -DWITH_IPP_A=OFF -DWITH_MATLAB=OFF -DWITH_VA=OFF -DWITH_VA_INTEL=OFF -DWITH_GDAL=OFF -DWITH_GPHOTO2=OFF -DWITH_LAPACK=OFF -DCMAKE_C_FLAGS="-Wno-error=address -Wno-pointer-to-int-cast -mno-avx -msse3 -mno-ssse3 -mno-sse4.1 -mno-sse4.2" -DCMAKE_CXX_FLAGS="-Wno-error=address -Wno-pointer-to-int-cast -mno-avx -msse3 -mno-ssse3 -mno-sse4.1 -mno-sse4.2" -DINSTALL_CREATE_DISTRIB=ON -DBUILD_opencv_photo=OFF ..
endef


define $(package)_build_cmds 
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install && mkdir -p $($(package)_staging_prefix_dir)/lib/pkgconfig && cp -n unix-install/opencv.pc $($(package)_staging_prefix_dir)/lib/pkgconfig/opencv.pc
endef
