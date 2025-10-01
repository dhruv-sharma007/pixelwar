# PixelWar - Cocos2d-x Project

## Prerequisites

1. **Install Cocos2d-x**
    - Download and install [Cocos2d-x](https://www.cocos.com/en/cocos2d-x).
    - Follow the official setup guide for your OS.

2. **Setup Environment Variables**
    - Add Cocos2d-x tools to your system PATH.

## Project Setup

1. **Create a New Cocos2d-x Project**
    ```sh
    cocos new PixelWar -p com.example.pixelwar -l cpp
    ```

2. **Copy Source Files**
    - Copy the `Classes` and `Resources` folders from this repository.
    - Paste them into your newly created project directory, replacing the existing folders.

## Build & Run

1. **Open Terminal/Command Prompt**
2. **Navigate to Project Directory**
    ```sh
    cd /path/to/PixelWar
    ```
3. **Build the Project**
    - For Windows:
      ```sh
      cocos run -p win32
      ```
    - For Mac:
      ```sh
      cocos run -p mac
      ```
    - For Android:
      ```sh
      cocos run -p android
      ```

## Notes

- Make sure all dependencies are installed (Visual Studio, Android Studio, Xcode, etc.).
- Refer to the [Cocos2d-x documentation](https://docs.cocos2d-x.org/) for troubleshooting.

## License

This project is licensed under the MIT License.