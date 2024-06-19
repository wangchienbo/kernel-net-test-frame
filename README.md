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

- 推荐使用`zsh`+`on my zsh`

## 使用xmake构建和运行项目

- 配置和构建整个项目
    ```shell
    xmake config -y
    xmake build
    ```
- 构建指定目标
    ```
    xmake build [target-name]
    ```
- 启动服务
    ```shell
    xmake run service
    ```
- 启动myTest中定义的测试
    ```shell
    xmake run myTest
    ```
- 格式化所有代码
    ```shell
    xmake format
    ```

