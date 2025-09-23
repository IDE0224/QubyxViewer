# QubyxViewer

A professional color management viewer application built with Qt. QubyxViewer provides advanced color calibration and display management capabilities for professional workflows.

## Features

- **Multi-Display Support**: Manage multiple displays with individual color profiles
- **ICC Profile Management**: Load and apply ICC color profiles
- **3DLUT Generation**: Generate and apply 3D Look-Up Tables for color transformation
- **Real-time Color Correction**: Live color correction and calibration
- **Professional Workflow**: Designed for color-critical applications
- **Cross-Platform**: Windows, macOS, and Linux support

## Prerequisites

### Required
- **Qt 5.15+ or Qt 6.x**: Download from [qt.io](https://www.qt.io/download)
- **Visual Studio 2019+** (Windows) or **Xcode** (macOS) or **GCC/Clang** (Linux)
- **Windows SDK 10.0+** (Windows)

### Optional
- **Qt Creator**: For development and debugging

## Building

### Method 1: Qt Creator (Recommended)

1. **Open Qt Creator**
2. **Open Project**: Select `QubyxViewer.pro`
3. **Configure**: Select your Qt version and compiler
4. **Build**: Press Ctrl+B or use the Build menu

### Method 2: Command Line (qmake)

1. **Open Qt Command Prompt** (from Start Menu)
2. **Navigate to the project directory:**
   ```cmd
   cd QubyxViewer
   ```
3. **Generate Makefile:**
   ```cmd
   qmake QubyxViewer.pro
   ```
4. **Build:**
   ```cmd
   nmake
   ```


## Usage

### Running the Application

```cmd
QubyxViewer.exe
```

### Basic Workflow

1. **Load Display Profiles**: Import ICC profiles for your displays
2. **Load Working Profile**: Set your working color space (e.g., sRGB, Adobe RGB)
3. **Generate 3DLUT**: Create color transformation tables
4. **Apply Calibration**: Apply color correction to your displays

### Supported File Formats

- **ICC Profiles**: `.icc`, `.icm`
- **3DLUT Files**: Various formats supported
- **Media Files**: Images and videos for testing

## Project Structure

```
QubyxViewer/
├── main.cpp                    # Application entry point
├── qubyxviewerdata.h/cpp       # Main application logic
├── LutGenerator.h/cpp          # 3DLUT generation
├── filereader.h/cpp            # File I/O operations
├── pixelhandler.h/cpp          # Pixel processing
├── qml/                        # QML user interface
├── libs/                       # External libraries
│   ├── QubyxProfile.h/cpp      # ICC profile handling
│   ├── DisplayInfo.h/cpp       # Display management
│   ├── ICCProfLib/             # ICC profile library
│   └── ...                     # Other dependencies
├── QubyxViewer.pro             # Qt project file
```

## Dependencies

### Internal Libraries (included)
- **QubyxProfile**: ICC profile handling and color management
- **DisplayInfo**: Display enumeration and management
- **ICCProfLib**: Open-source ICC profile library
- **Eigen**: Mathematical operations and matrix calculations

### External Dependencies
- **Qt Framework**: GUI, multimedia, and QML support
- **Windows SDK**: Windows-specific APIs (Windows only)

## Troubleshooting

### Build Issues

1. **Qt not found**: Install Qt and use Qt Command Prompt or Qt Creator
2. **Compiler not found**: Install Visual Studio Build Tools and use Developer Command Prompt
3. **Missing libraries**: Ensure all required Windows SDK components are installed
4. **Environment conflicts**: Use separate command prompts (Qt Command Prompt OR Visual Studio Developer Command Prompt, not both)

### Runtime Issues

1. **DLL not found**: Ensure Qt libraries are in PATH or copy to application directory
2. **Display detection fails**: Run as administrator or check display drivers
3. **Profile loading fails**: Verify ICC profile files are valid

## Development

### Adding New Features

1. **UI Changes**: Modify QML files in `qml/` directory
2. **Core Logic**: Update C++ files in the root directory
3. **Library Functions**: Add to appropriate files in `libs/` directory

### Debugging

1. **Qt Creator**: Use built-in debugger
2. **Visual Studio**: Attach to running process
3. **Console Output**: Check for error messages and warnings

## License

This project is licensed under the GNU General Public License v3.0 - see the LICENSE file for details.

## Support

- **Issues**: Report bugs and request features on GitHub
- **Email**: support@qubyx.com
- **Website**: https://qubyx.com
