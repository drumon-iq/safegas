#
#
from flask import Flask
from flask_cors import CORS
from random import randrange

app = Flask(__name__)
CORS(app)

@app.route("/")
def hello_world():
    return "bitchlesslarry"

@app.route("/bitchlesslarry")
def lary():
    return "https://c.tenor.com/PPAVaj0CzTMAAAAC/tenor.gif"

@app.route("/peenarexplosionchamber")
def penar():
    return "https://i.kym-cdn.com/photos/images/newsfeed/002/635/266/7f8"

@app.route("/valveValue")
def toggleValve():
    return str(randrange(0,2));

@app.route("/gasValue")
def gaslValue():
    return str(randrange(0,1000));

@app.route("/flameValue")
def flameValue():
    return str(randrange(0,1000));
