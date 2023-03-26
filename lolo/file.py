import cgi

# Define HTML template for file upload form
HTML_TEMPLATE = '''
<html>
<head><title>File Upload</title></head>
<body>
<form method="post" enctype="multipart/form-data">
  <input type="file" name="file">
  <input type="submit" value="Upload">
</form>
</body>
</html>
'''

def main():
    # Print HTML form to response
    print('Content-Type: text/html\n')
    print(HTML_TEMPLATE)

    # Check if form is submitted
    form = cgi.FieldStorage()
    if 'file' in form:
        # Get uploaded file
        fileitem = form['file']

        # Check if file was uploaded
        if fileitem.filename:
            # Open a new file in write-binary mode
            with open(fileitem.filename, 'wb') as f:
                # Copy the contents of the uploaded file into the new file
                f.write(fileitem.file.read())

            # Print success message
            print('File {} has been uploaded.'.format(fileitem.filename))
        else:
            print('No file was uploaded.')
    else:
        print('Please select a file to upload.')

if __name__ == '__main__':
    main()
