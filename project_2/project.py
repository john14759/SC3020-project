import tkinter as tk
from PIL import Image, ImageTk
from interface import create_scrollable_canvas, create_legend, resize_image, \
    open_fullsize_image, view_statement_details
from explore import connect_to_db, get_block_size, get_buffer_size, get_qep_image, \
                 close_db_connection, get_qep_statements
from threading import Thread
import tkinter.font as tkFont

# Global variables
global create_legend_flag, legend_canvas, click_instruction_label
legend_canvas = None
create_legend_flag = False
click_instruction_label = None

# Function to execute the SQL query
def execute_sql_query():
    global click_instruction_label
    # Function to execute the SQL query in a separate thread
    def execute_query_thread():
        global click_instruction_label
        try:
            connect_to_db()

            # Fetch the QEP image
            qep_digraph = get_qep_image(query)

            # Check if QEP is None, indicating an invalid query
            if qep_digraph is None:
                result_label.config(text="Error: Invalid query. Please check your SQL syntax.")
                return

            statements, details = get_qep_statements()
            buffer_size = get_buffer_size()
            blk_size = get_block_size()
            
            close_db_connection()

            # Save the QEP digraph as a PNG file
            qep_digraph.format = 'png'
            qep_digraph.render(filename="qep_tree")

            # Open the QEP image and convert it to Tkinter PhotoImage
            qep_image = Image.open("qep_tree.png")

            max_dimensions = (600, 600)  # Maximum dimensions for the image
            resized_qep_img = resize_image("qep_tree.png", max_dimensions)

            qep_image = ImageTk.PhotoImage(resized_qep_img)
            qep_label.bind("<Button-1>", lambda e: open_fullsize_image())

            qep_label.config(image=qep_image)
            qep_label.image = qep_image
            qep_label.pack(side="top", fill="both", expand=True)

            # Define a bold font
            bold_font = tkFont.Font(family="Helvetica", size=10, weight="bold")

            # Check if the label already exists, if not create it
            if click_instruction_label is None:
                click_instruction_label = tk.Label(qep_label.master, text="Click on the image to view it in full size", font=bold_font)
                click_instruction_label.pack(side="top")
            else:
                # Optionally update the label text or properties if needed
                click_instruction_label.config(text="Click on the image to view it in full size", font=bold_font)

            # Update the statements in the right frame
            analysis_output_label.config(text='\n'.join(statements), font=("Helvetica", 10))
            analysis_output_label.pack(side="top", fill="both", expand=True)
            for widget in right_frame.winfo_children():
                if (isinstance(widget, tk.Button)):
                    widget.destroy()
            for i, detail in enumerate(details):
                button = tk.Button(right_frame, text=f"Step {i+1} Details", command=lambda s=detail: view_statement_details(window, s))
                button.pack()
            create_legend(left_frame, legend_items)

        except Exception as e:
            result_label.config(text=f"Error: {str(e)}")

    # Get the query from the entry field
    query = sql_entry.get()

    # Create a thread to execute the query
    query_thread = Thread(target=execute_query_thread)
    query_thread.start()

# Create the main window
window = tk.Tk()
window.title("SQL Query Executor")

# Set the window size to cover the entire screen
window.geometry(f"{int(4/5*window.winfo_screenwidth())}x{window.winfo_screenheight()}")

# Create a top canvas for title, entry field, and button
top_canvas = tk.Canvas(window)
top_canvas.pack(side=tk.TOP, padx=10, pady=10)

# Create a title label with a big font in the top canvas
title_label = tk.Label(top_canvas, text="Enter SQL Query", font=("Helvetica", 15))
title_label.pack()

# Create an entry field for the SQL query in the top canvas
sql_entry = tk.Entry(top_canvas, width=50, font=("Helvetica", 10))
sql_entry.pack(pady=10)

# Create a button to execute the SQL query in the top canvas
execute_button = tk.Button(top_canvas, text="Execute Query", command=execute_sql_query, font=("Helvetica", 12))
execute_button.pack(pady=10)

# Create scrollable left and right canvases
left_canvas, left_frame = create_scrollable_canvas(window, side=tk.LEFT, min_width=400)
right_canvas, right_frame = create_scrollable_canvas(window, side=tk.RIGHT, min_width=400)

# Create a label to display the QEP analysis output in the right canvas
analysis_output_label = tk.Label(right_frame, text="", font=("Helvetica", 12), justify=tk.LEFT, wraplength=550)
analysis_output_label.place()

# Legend items
legend_items = [
    {"text": "Start-up cost: Estimated units to start up a node to start a query"},
    {"text": "Total cost: Estimated units to finish processing and return results."},
    {"text": "Shared Hit Blocks: Number of shared blocks read into cache"},
    {"text": "Local Hit Blocks: Number of local blocks read into cache"},
]

# Create a label to display the QEP image in the left canvas
qep_label = tk.Label(left_frame, font=("Helvetica", 12))
qep_label.place()

# Create a label to display the result in the left canvas
result_label = tk.Label(left_frame, text="", font=("Helvetica", 12))
result_label.place()

# Start the mainloop
window.mainloop()