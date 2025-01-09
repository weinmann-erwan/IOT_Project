# IOT Project

## Getting Started

### Clone the Repository

First, clone the GitHub repository to your local machine:

```bash
git clone https://github.com/weinmann-erwan/IOT_Project.git
```

### Import Node-RED Flow

1. Open Node-RED.
2. Import the `node-red` file from the Node-red folder to get the desired layout.
3. Install the `dashboard 2.0` extension in Node-RED.

### MongoDB Access

Ensure you have access to the MongoDB database. If you do not have access, please request it via email: [dana.dacosta.001@student.uni.lu](mailto:dana.dacosta.001@student.uni.lu).

### Start the Server

1. Open a new terminal.
2. Navigate to the `src` directory:

    ```bash
    cd src
    ```

3. Start the server:

    ```bash
    node server.js
    ```

### Server Options

Once the server is successfully started, it will prompt you with two options:

1. **Register a Room**: Select this option by entering `1`. The server will check if the Arduino is connected. If the Arduino is connected, it will register a room with the settings from the Arduino. If not, it will notify you that the Arduino is missing and return to the main menu.
2. **Launch Server for Node-RED**: Select this option by entering `2`. This will make the server available for Node-RED.

### Using Node-RED

1. Open Node-RED in a separate terminal and launch the URL.
2. Once in Node-RED and all extensions are downloaded, open the dashboard.
3. Use the sliders to set your preferences. The website will then provide you with a list of rooms sorted from most recommended to least recommended based on your preferences.
