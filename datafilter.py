def filter():
    f = open("data/marketing_sample.csv", "r+", encoding='utf-8')
    content = f.read()
    f.seek(0)
    for char in content:
        if(ord(char)<128 and ord(char) >31 or char == '\n'):
            f.write(char)
    f.truncate()
    f.close()
        
if __name__ == "__main__":
    filter()