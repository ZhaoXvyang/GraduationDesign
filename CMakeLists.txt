# 设置最低 CMake 版本要求
cmake_minimum_required(VERSION 3.16)

# 设置项目名称和版本号
project(GraduationDesign VERSION 0.1 LANGUAGES CXX)

# 开启自动处理 .ui 文件 (将其转换为 C++ 代码)
set(CMAKE_AUTOUIC ON)
# 开启自动处理 .moc 文件 (自动生成元对象代码)
set(CMAKE_AUTOMOC ON)
# 开启自动处理 .qrc 文件 (资源文件)
set(CMAKE_AUTORCC ON)

# 设置 C++ 语言标准为 C++17，并强制要求使用
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Qt 组件：Widgets 和 Charts
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)
# 确保找到正确版本的 Qt
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

# 定义项目的源文件列表，包括 .cpp 和 .ui 文件
set(PROJECT_SOURCES
    main.cpp            # 主程序文件
    widget.cpp          # 主窗口源文件
    widget.h            # 主窗口头文件
    widget.ui           # 主窗口 UI 文件
)

# 如果使用 Qt6，则使用 qt_add_executable 来创建可执行文件
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GraduationDesign
        MANUAL_FINALIZATION   # 手动控制 finalization
        ${PROJECT_SOURCES}     # 添加项目源文件
        drawdata.h
        drawdata.cpp


    )
# 如果使用 Qt5 或其他版本，则使用传统的 add_executable
else()
    if(ANDROID)
        add_library(GraduationDesign SHARED
            ${PROJECT_SOURCES}  # 为 Android 构建共享库
        )
    else()
        add_executable(GraduationDesign
            ${PROJECT_SOURCES}  # 创建普通的可执行文件
        )
    endif()
endif()

# 将 Qt 的必要组件链接到目标：Widgets 和 Charts
target_link_libraries(GraduationDesign PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)

# 设置 macOS 或 iOS 平台的打包标识符
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.GraduationDesign)
endif()

# 设置目标的属性，尤其是打包相关的属性
set_target_properties(GraduationDesign PROPERTIES
    ${BUNDLE_ID_OPTION}  # 设置 macOS 包的唯一标识符
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}  # 设置版本号
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}  # 设置短版本号
    MACOSX_BUNDLE TRUE  # 标记为 macOS 应用程序包
    WIN32_EXECUTABLE TRUE  # 标记为 Windows 可执行文件
)

# 设置安装路径
include(GNUInstallDirs)
install(TARGETS GraduationDesign
    BUNDLE DESTINATION .  # macOS 的安装目录
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}  # 库文件的安装目录
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # 可执行文件的安装目录
)

# 完成 Qt6 的可执行文件构建
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GraduationDesign)
endif()
