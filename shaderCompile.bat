@echo off
echo Searching for .frag and .vert files in the current directory and subdirectories...
echo.

for /r %%f in (*.frag *.vert) do (
    if /i "%%~xf" == ".vert" (
        echo Compiling vertex shader: %%~f
        C:/VulkanSDK/1.3.296.0/Bin/glslc.exe "%%f" -o "%%~dpnfvert.spv"
    ) else (
        echo Compiling fragment shader: %%~f
        C:/VulkanSDK/1.3.296.0/Bin/glslc.exe "%%f" -o "%%~dpnffrag.spv"
    )
)

echo Search complete.
pause