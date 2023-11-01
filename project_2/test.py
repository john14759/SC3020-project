import psycopg2

def connection_to_db():    
    try:
        # Define the connection parameters
        dbname = "TPC-H"
        user = "postgres"
        password = "voidbeast1"
        host = "127.0.0.1"
        port = "5432"  # Default PostgreSQL port is 5432

        # Create a connection to the database
        connection = psycopg2.connect(
            dbname=dbname,
            user=user,
            password=password,
            host=host,
            port=port
        )

        # Create a cursor
        cursor = connection.cursor()

        # Now, you have a working database connection and a cursor for executing SQL queries.

        return connection, cursor

    except psycopg2.Error as e:
        print(f"Error connecting to the database: {e}")