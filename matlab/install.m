% Install compiled MEX files and rumtime support libraries.

clear all; clc;

BUILD_DIR       = '../build';
BUILD_BIN_DIR   = fullfile(BUILD_DIR, 'bin');
BUILD_MEX_DIR   = fullfile(BUILD_DIR, 'mex');

MEX_DIR         = './util';
OPENCV_DLL_DIR  = '../msvc/opencv-2.4.12/x64/vc14/bin';
OPENCV_DLL_LIST = {'opencv_core2412.dll' 'opencv_highgui2412.dll' 'opencv_imgproc2412.dll'};

if ispc
    BUILD_BIN_DIR = fullfile(BUILD_BIN_DIR, 'Release');
    BUILD_MEX_DIR = fullfile(BUILD_MEX_DIR, 'Release');
end

mex_file_list = dir(fullfile(BUILD_MEX_DIR, ['*.' mexext]));
if isempty(mex_file_list)
    error('Error. \nMex files not found in %s.\n', BUILD_MEX_DIR);
end

for i = 1:length(mex_file_list)
    mex_file_name = mex_file_list(i).name;
    copyfile(fullfile(BUILD_MEX_DIR, mex_file_name), MEX_DIR);
    fprintf('Copying MEX file: %s ...\n', mex_file_name);
end

if ispc
    dll_file_list = dir(fullfile(BUILD_BIN_DIR, '*.dll'));
    for i = 1:length(dll_file_list)
        dll_file_name = dll_file_list(i).name;
        copyfile(fullfile(BUILD_BIN_DIR, dll_file_name), MEX_DIR);
        fprintf('Copying DLL file: %s ...\n', dll_file_name);
    end
    
    for i = 1:length(OPENCV_DLL_LIST)
        file_name = OPENCV_DLL_LIST{i};
        copyfile(fullfile(OPENCV_DLL_DIR, file_name), MEX_DIR);
        fprintf('Copying OPENCV DLL file: %s ...\n', file_name);
    end
end

fprintf('Finished.\n');

