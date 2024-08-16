#pragma once

class SerialLineReader{
    private:
        Stream &stream;
        bool has_line = false;
        String line;
        void try_read(){
            while(stream.available()){
                char ch = stream.read();
                if(ch == '\r' || ch == '\n'){
                    has_line = true;
                    return;
                }else{
                    line += ch;
                }
            }
        }
    public:
        SerialLineReader(Stream &stream) : stream(stream){
        }
        bool available(){
            if(!has_line){
                try_read();
            }
            return has_line;
        }

        String readLine(){
            assert(has_line);
            has_line = false;
            return line;
        }
};