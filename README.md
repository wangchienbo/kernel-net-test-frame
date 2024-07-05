# Kernel Net Test Frame

## 环境配置
- 安装`xmake`
    ```shell
    curl -fsSL https://xmake.io/shget.text | bash
    ```

- 安装`clangd`作为`VScode`的`C++`提示插件

- 运行以下命令
    ```shell
    xmake project -k compile_commands
    ```
    生成编译命令供`clangd`使用

## 使用xmake构建和运行项目

- 配置和构建整个项目
    ```shell
    xmake config -y
    xmake build
    ```
- 构建和运行指定目标
    ```shell
    xmake build [target-name]
    xmake run [target-name]
    ```
- 调试代码
    ```
    xmake config -m debug
    xmake
    xmake run -d [target-name]
    ```
- 启动server
    ```shell
    xmake run server
    ```
- 启动client
    ```shell
    xmake run client
    ```
- 启动test文件夹中xxx.cpp定义的测试
    ```shell
    xmake run xxx
    ```
- 格式化所有代码
    ```shell
    xmake format
    ```

