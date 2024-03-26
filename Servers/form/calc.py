import sys
print('This is an error message', file=sys.stderr)

body = "<!DOCTYPE html><html><head><title>calc</title></head><body>"
body += "<div class=\"calc\" style=\"border:1px solid black; border-radius: 25% 10%; align-items: center; justify-content: center; text-align: center; background-color: grey; width: 300px; height: 300px;\">"
body += "<form method=\"post\"><label for=\"number\">First Number:</label>"
body += "Goodbye Cruel World!!!!!!!!!!!\n"
body += "</div>"
body += "</body></html>"
header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\ncharset=utf-8\r\n"
header += "Content-Length: " + str(len(body)) + "\r\n\r\n"
print (header + body)