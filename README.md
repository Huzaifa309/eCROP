# eCROP Engine

A high-performance, asynchronous engine for eCROP, built with modern C++17. The engine leverages [Aeron](https://github.com/aeron-io/aeron) for ultra-low-latency messaging, PostgreSQL for robust identity data storage, and SBE (Simple Binary Encoding) for efficient message serialization. The modular design includes custom wrapper libraries for enhanced performance and maintainability.

---

## Features

- **Aeron UDP Messaging:** Subscribes and publishes to Aeron UDP channels for fast, reliable message transport.
- **PostgreSQL Integration:** Connects to PostgreSQL database for identity verification against stored records.
- **SBE Message Processing:** Uses Simple Binary Encoding for efficient serialization/deserialization of identity messages.
- **Identity Verification Workflow:** Processes "Identity Verification Request" messages and validates against database records.
- **Asynchronous Processing:** Uses background threads for message polling and processing.
- **Custom Logging:** Integrates a custom logger for file-based, level-controlled logging with fast logging capabilities.
- **Database Connection Management:** Maintains persistent PostgreSQL connections with error handling.

---

## Requirements

- **C++17** or newer
- **CMake** 3.16 or newer
- **PostgreSQL** development libraries (`libpqxx`)
- **Aeron** (fetched and built automatically via CMake)
- **Java** (for SBE code generation)

## Dependencies

This project requires the following custom wrapper libraries:

### 1. **Aeron Wrapper**
- **Repository:** [https://github.com/ehsanrashid/aeronWrapper](https://github.com/ehsanrashid/aeronWrapper)
- **Description:** Modern C++17 wrapper for Aeron high-performance messaging library
- **Features:** Simplified API, enhanced error handling, background polling, RAII compliance
- **Installation:** Follow build instructions in the repository README

### 2. **PostgreSQL Wrapper**
- **Repository:** [https://github.com/ehsanrashid/pgWrapper](https://github.com/ehsanrashid/pgWrapper)
- **Description:** C++ wrapper for PostgreSQL database operations
- **Features:** Connection pooling, prepared statements, transaction management
- **Installation:** Follow build instructions in the repository README

### 3. **Logger Library**
- **Repository:** [https://github.com/Huzaifa309/loggerLib](https://github.com/Huzaifa309/loggerLib)
- **Description:** High-performance logging library with file rotation and multiple log levels
- **Features:** Fast logging, file rotation, configurable log levels, thread-safe operations
- **Installation:** Follow build instructions in the repository README

---

## Database Setup

1. **Install PostgreSQL:**
   ```sh
   sudo apt-get install postgresql postgresql-contrib libpqxx-dev
   ```

2. **Create database and user:**
   ```sql
   -- As postgres user
   CREATE DATABASE ekycdb;
   CREATE USER huzaifa WITH PASSWORD '3214';
   GRANT ALL PRIVILEGES ON DATABASE ekycdb TO huzaifa;
   ```

3. **Create users table:**
   ```sql
   -- Connect to ekycdb
   \c ekycdb
   
   CREATE TABLE users (
       id SERIAL PRIMARY KEY,
       type VARCHAR(20),
       identity_number VARCHAR(20) NOT NULL,
       name VARCHAR(100) NOT NULL,
       date_of_issue DATE,
       date_of_expiry DATE,
       address TEXT,
       logged_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
   );
   
   -- Insert sample data
   INSERT INTO users (type, identity_number, name, date_of_issue, date_of_expiry, address)
   VALUES ('cnic', '4210109729681', 'Huzaifa Ahmed', '2020-01-15', '2030-01-15', 'Gulshan-e-Iqbal, Block 2');
   ```

---

## Quick Start

### Automated Installation (Recommended)

For quick setup, you can use this automated installation script:

```sh
#!/bin/bash
# install_dependencies.sh

set -e

echo "Installing system dependencies..."
sudo apt-get update
sudo apt-get install -y build-essential cmake git openjdk-11-jdk libpqxx-dev postgresql postgresql-contrib

echo "Building and installing Logger Library..."
git clone https://github.com/Huzaifa309/loggerLib.git
cd loggerLib && mkdir build && cd build
cmake .. && make -j$(nproc) && sudo make install
cd ../..

echo "Building and installing Aeron Wrapper..."
git clone https://github.com/ehsanrashid/aeronWrapper.git
cd aeronWrapper && mkdir build && cd build
cmake .. && make -j$(nproc) && sudo make install
cd ../..

echo "Building and installing PostgreSQL Wrapper..."
git clone https://github.com/ehsanrashid/pgWrapper.git
cd pgWrapper && mkdir build && cd build
cmake .. && make -j$(nproc) && sudo make install
cd ../..

echo "Updating library cache..."
sudo ldconfig

echo "Cloning and building eCROP..."
git clone https://github.com/Huzaifa309/eCROP
cd eCROP && mkdir build && cd build
cmake .. && make -j$(nproc)

echo "Installation complete! Run './eCROP' to start the engine."
```

Save as `install_dependencies.sh`, make executable, and run:
```sh
chmod +x install_dependencies.sh
./install_dependencies.sh
```

---

## Manual Build Instructions

### Prerequisites Installation

1. **Install system dependencies:**
   ```sh
   # Ubuntu/Debian
   sudo apt-get update
   sudo apt-get install build-essential cmake git openjdk-11-jdk libpqxx-dev postgresql postgresql-contrib
   
   # CentOS/RHEL
   sudo yum install gcc-c++ cmake git java-11-openjdk-devel libpqxx-devel postgresql postgresql-server
   ```

### Dependency Installation

2. **Build and install Logger Library:**
   ```sh
   git clone https://github.com/Huzaifa309/loggerLib.git
   cd loggerLib
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   sudo make install
   cd ../..
   ```

3. **Build and install Aeron Wrapper:**
   ```sh
   git clone https://github.com/ehsanrashid/aeronWrapper.git
   cd aeronWrapper
   mkdir build && cd build
   crm -rf build && mkdir build && cd build
   cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<path_to_aeronWrapper/install>
   cmake --build . -j$(nproc)
   cmake --install .
   cd ../..
   ```

4. **Build and install PostgreSQL Wrapper:**
   ```sh
   git clone https://github.com/ehsanrashid/pgWrapper.git
   cd pgWrapper
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   sudo make install
   cd ../..
   ```

### Project Build

5. **Clone the eCROP repository:**
   ```sh
   git clone https://github.com/Huzaifa309/eCROP
   cd eCROP
   ```

6. **Generate SBE messages** (if schema changes):
   ```sh
   java -jar sbe-all-1.36.0-SNAPSHOT.jar login-schema.xml
   ```

7. **Build the project:**
   ```sh
   mkdir build
   cd build
   cmake .. -DCMAKE_PREFIX_PATH=<path_to_aeronWrapper/install>
   make -j$(nproc)
   ```

### Verification

8. **Verify the build:**
   ```sh
   ./eCROP            # Check if binary runs (no --version flag)
   ldd ./eCROP        # Check library dependencies
   ```

---

## Usage

1. **Ensure PostgreSQL is running:**
   ```sh
   sudo systemctl start postgresql
   ```

2. **Run the executable:**
   ```sh
   ./eCROP
   ```

3. The engine will:
   - Connect to PostgreSQL database (`ekycdb`)
   - Connect to Aeron Media Driver
   - Start background message processing
   - Begin listening for identity verification requests

4. **To stop the engine:** Use Ctrl+C or stop the process.

---

## Message Processing Workflow

1. **Receive Message:** Engine receives SBE-encoded `IdentityMessage` on Aeron subscription channel
2. **Decode Message:** Extracts identity information (name, ID, type, etc.)
3. **Check Message Type:** Processes "Identity Verification Request" and "Add User in System" messages with `verified=false`
4. **Database Operations:** 
   - For verification requests: Queries PostgreSQL to verify identity against stored records
   - For add user requests: Inserts new user records into the database
5. **Log Results:** Logs verification/addition success/failure
6. **Response:** Send back verification/addition result message via Aeron publication

### Sample Identity Message Fields:
- `msg`: "Identity Verification Request"
- `type`: "cnic" or "passport"
- `id`: Identity number (e.g., "4210109729681")
- `name`: Full name (e.g., "Huzaifa Ahmed")
- `dateOfIssue`: Issue date
- `dateOfExpiry`: Expiry date
- `address`: Address information
- `verified`: "true" or "false"

---

## Configuration

- **Database Connection:**
  - Host: `localhost`
  - Port: `5432`
  - Database: `ekycdb`
  - User: `huzaifa`
  - Password: `3214`

- **Aeron Channels:**
  - Subscription: `aeron:udp?endpoint=0.0.0.0:50000`, Stream ID: `1001`
  - Publication: `aeron:udp?endpoint=anas.eagri.com:10001`, Stream ID: `1001`

- **Log Files:**
  - Format: `Gateway_SBE_<timestamp>.log`
  - Location: `build/logs/`
  - Max size: 10 MB

---

## Project Structure

```
.
├── CMakeLists.txt
├── login-schema.xml       # SBE schema definition
├── sbe-all-1.36.0-SNAPSHOT.jar  # SBE code generator
├── include
|   ├── eCROPEngine.h       # Engine class definition
|   ├── helper.h           # Helper functions
|   └── messages/          # Generated SBE message classes
|       ├── IdentityMessage.h
|       ├── MessageHeader.h
|       └── Char64str.h
├── src
|   ├── eCROPEngine.cpp     # Engine class implementation
|   ├── main.cpp           # Application entry point
└── build/
    └── logs/              # Log output directory
```

---

## Performance Optimization

For high-throughput scenarios (>1000 requests/second), consider these optimizations:

### Database Optimizations
```sql
-- Create performance indexes
CREATE INDEX CONCURRENTLY idx_users_identity_name ON users(identity_number, name);
CREATE INDEX CONCURRENTLY idx_users_identity ON users(identity_number);

-- Analyze table statistics
ANALYZE users;
```

### Application-Level Optimizations
- **Connection Pooling:** Use pgWrapper's connection pooling for concurrent database access
- **Prepared Statements:** Utilize prepared statements for repeated queries
- **Caching:** Implement LRU cache for frequently verified identities
- **Async Processing:** Leverage aeronWrapper's background polling for non-blocking operations
- **Logging:** Disable detailed logging in production (comment out Log statements)

### System-Level Optimizations
```sh
# Increase file descriptor limits
echo "* soft nofile 65536" >> /etc/security/limits.conf
echo "* hard nofile 65536" >> /etc/security/limits.conf

# Optimize PostgreSQL settings
echo "shared_buffers = 256MB" >> /etc/postgresql/*/main/postgresql.conf
echo "effective_cache_size = 1GB" >> /etc/postgresql/*/main/postgresql.conf
```

### Expected Performance
- **Without optimization:** ~25 requests/second (40s for 1000 requests)
- **With database indexes:** ~100-200 requests/second (5-10s for 1000 requests)
- **With full optimization:** >500 requests/second (<2s for 1000 requests)

---

## Architecture

The eCROP Engine follows a modern, asynchronous architecture:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Client Apps   │    │  Message Queue  │    │  eCROP Engine   │
│                 │───▶│     (Aeron)     │───▶│                 │
│ - Web Services  │    │  UDP Messaging  │    │ - Message Proc. │
│ - Mobile Apps   │    │  Low Latency    │    │ - DB Queries    │
│ - APIs          │    │                 │    │ - Response Gen. │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                                       │
                                                       ▼
                                               ┌─────────────────┐
                                               │   PostgreSQL    │
                                               │   Database      │
                                               │ - User Records  │
                                               │ - Identity Data │
                                               └─────────────────┘
```

### Component Responsibilities
- **aeronWrapper:** High-performance messaging with automatic retry and connection management
- **pgWrapper:** Database operations with connection pooling and prepared statements
- **quillLogger:** Thread-safe, high-performance logging with file rotation
- **SBE Messages:** Zero-copy serialization for maximum throughput

---

## Development

### Adding New Message Types
1. Update `login-schema.xml` with new message schema
2. Regenerate SBE classes: `java -jar sbe-all-1.36.0-SNAPSHOT.jar login-schema.xml`
3. Update message processing logic in `verify_and_respond()`

### Database Schema Changes
- Modify table structure as needed
- Update SQL queries in `user_exists()` method
- Test with sample data

---

## Troubleshooting

### Database Issues
- **Database Connection Failed:**
  - Verify PostgreSQL is running: `sudo systemctl status postgresql`
  - Check database credentials and permissions
  - Ensure `ekycdb` database exists
  - Verify `pgWrapper` library is installed: `ldconfig -p | grep pgWrapper`

### Build Issues
- **Missing Dependencies:**
  - Ensure all wrapper libraries are installed in `/usr/local/lib/`
  - Run `sudo ldconfig` after installing libraries
  - Check header files exist in `/usr/local/include/`

- **SBE Compilation Errors:**
  - Verify Java is installed: `java -version`
  - Check generated message classes in `include/` directory
  - Regenerate SBE classes if schema was modified

- **CMake Configuration Failed:**
  - Verify CMake version: `cmake --version` (requires 3.16+)
  - Clear build directory and retry: `rm -rf build && mkdir build`
  - Check that wrapper libraries are properly installed

### Runtime Issues
- **Aeron Connection Issues:**
  - Ensure Aeron Media Driver is accessible
  - Check network connectivity for UDP endpoints
  - Verify `aeronWrapper` library is linked: `ldd ./eCROP | grep aeron`

- **Logger Not Found:**
  - Verify loggerLib is installed: `ls /usr/local/lib/libquillLogger*`
  - Check header exists: `ls /usr/local/include/loggerlib*`
  - Ensure library is linked: `ldd ./eCROP | grep libquillLogger`

### Performance Issues
- **Slow Database Queries:**
  - Create database indexes: `CREATE INDEX idx_users_identity_name ON users(identity_number, name);`
  - Monitor PostgreSQL performance: `sudo -u postgres psql -c "SELECT * FROM pg_stat_activity;"`
  - Consider connection pooling for high-throughput scenarios

### Quick Dependency Check
```sh
# Verify all dependencies are installed
ls /usr/local/lib/lib{aeronWrapper,pgWrapper,quillLogger}*
ls /usr/local/include/{aeron_wrapper,pg_wrapper,loggerlib}*
```

---
