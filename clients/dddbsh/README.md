# Get Started

1. Change to the `clients/dddbsh` directory:

    ```bash
    cd clients/dddbsh
    ```

2. Install dependencies:

    ```bash
    npm install
 
    ```
3. Install node dddb driver
   
   Make sure to setup the driver locally before! ([`drivers/nodejs/dddb/README.md`](drivers/nodejs/dddb/README.md))
   
   ```bash
    npm link dddb
    ```

4. Link the package globally:

    ```bash
    npm link
    ```

You can now run `dddbsh` from any terminal:

```bash
dddbsh
```