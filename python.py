import serial
import mysql.connector
from mysql.connector import Error
import time
import logging

# Configuration de la connexion série
SERIAL_PORT = 'COM7'  #Port série correct de l'ESP (e.g., 'COM3', '/dev/ttyUSB0', etc.)
BAUD_RATE = 115200

# Configuration de la connexion MySQL
MYSQL_HOST = 'localhost'
MYSQL_DATABASE = 'chute'
MYSQL_USER = 'root'
MYSQL_PASSWORD = 'tpuser'
MYSQL_PORT = 3308  # Spécifiez le port MySQL ici

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def create_connection():
    try:
        connection = mysql.connector.connect(
            host=MYSQL_HOST,
            port=MYSQL_PORT,
            database=MYSQL_DATABASE,
            user=MYSQL_USER,
            password=MYSQL_PASSWORD
        )
        if connection.is_connected():
            logging.info("Connected to MySQL database")
        return connection
    except mysql.connector.Error as e:
        logging.error(f"Error while connecting to MySQL: {e}")
        return None

def insert_message(connection, message):
    try:
        cursor = connection.cursor()
        sql_insert_query = "INSERT INTO messages (message) VALUES (%s)"
        cursor.execute(sql_insert_query, (message,))
        connection.commit()
        logging.info(f"Message inserted: {message}")
    except mysql.connector.Error as e:
        logging.error(f"Failed to insert record into MySQL table: {e}")

def main():
    while True:
        ser = None
        connection = None
        try:
            # Initialiser la connexion série
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
            logging.info(f"Connected to serial port {SERIAL_PORT}")

            # Initialiser la connexion MySQL
            connection = create_connection()
            if connection is None:
                time.sleep(5)  # Attendre 5 secondes avant de réessayer
                continue

            while True:
                if ser.in_waiting > 0:
                    message = ser.readline().decode('utf-8').strip()
                    if message:
                        insert_message(connection, message)

        except serial.SerialException as e:
            logging.error(f"Serial port error: {e}")
            if ser is not None and ser.is_open:
                ser.close()
            if connection is not None and connection.is_connected():
                connection.close()
            time.sleep(5)  # Attendre 5 secondes avant de réessayer

        except mysql.connector.Error as e:
            logging.error(f"MySQL error: {e}")
            if connection is not None and connection.is_connected():
                connection.close()
            time.sleep(5)  # Attendre 5 secondes avant de réessayer

        finally:
            if ser is not None and ser.is_open:
                ser.close()
            if connection is not None and connection.is_connected():
                connection.close()

if __name__ == '__main__':
    main()
