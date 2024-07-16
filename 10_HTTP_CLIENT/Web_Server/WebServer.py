from flask import Flask,request,render_template,json,jsonify,send_file
# __name__ 代表当前文件 
app = Flask(__name__)


@app.route('/',methods = ["GET"])
def route_path():
    return render_template("index.html")


@app.route('/post',methods = ["POST"])
def post_path():
    data = request.json
    print("Received JSON data:", data)
    response = {'message': 'OK', 'data': data}
    return jsonify(response)



@app.route('/load',methods = [ "GET"])
def load_path():
    file_path = '10_HTTP.bin'
    return send_file(file_path, as_attachment=True)



if __name__ == '__main__':
    app.run(debug=True, host='192.168.31.125', port = 8000)