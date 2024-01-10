import socket

# Khởi tạo socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Kết nối đến server
server_address = ('127.0.0.1', 8888)  # Địa chỉ và cổng của server
client_socket.connect(server_address)

print("Connected to server.")

while True:
    # Nhập thông điệp từ bàn phím
    message = input("Enter your message (type QUIT to exit): ")

    # Gửi dữ liệu đến server
    client_socket.send(message.encode('utf-8'))

    if message.upper() == 'QUIT':
        # Nếu người dùng nhập "QUIT", đóng kết nối và thoát khỏi vòng lặp
        print("Closing connection...")
        client_socket.close()
        break

    # Nhận dữ liệu từ server
    data = client_socket.recv(1024)
    print("Received from server:", data.decode('utf-8'))

    # Kiểm tra nếu là chuỗi "QUIT" thì đóng kết nối và thoát khỏi vòng lặp
    if data.decode('utf-8').upper() == 'QUIT':
        print("Closing connection...")
        client_socket.close()
        break
