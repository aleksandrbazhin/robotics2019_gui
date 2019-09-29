from bottle import Bottle, run

app = Bottle()

@app.route('/set/<value>')
def hello(value = ''):
    print ("received " + value)
    return "OK"

run(app, host='localhost', port=80)
