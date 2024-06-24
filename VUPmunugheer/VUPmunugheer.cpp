#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <string>
#include <cmath>

using namespace std;
namespace fs = std::experimental::filesystem;

string root;
unsigned int filelist[65536][2];
short proc = 0;

unsigned char key[65536];
unsigned short key_lenght, key_pos, enc_type;

unsigned int choise(unsigned short max) {
    string choise;
    unsigned char result;
    while (true) {
        result = 0;
        cin >> choise;
        for (int i = 0; i != choise.size(); i++) {
            if (choise[i] > 47 and choise[i] < 58) {
                result = result * 10 + choise[i] - 48;
            }
            else { break; }
        }
        if (result > 0 and result <= max) { return result-1; }
        cout << "Такого варинта нет! Введите число от 1 до " << max << "!\n";
    }
}

void key_transform(string s_key) {
    int i, k=0;
    for (i = s_key.length()-1; i >= 0; i-=2) {
        if (s_key[i] > 47 and s_key[i] < 58) {
            key[k] = s_key[i] - 48;
        }
        else if (s_key[i] > 96 and s_key[i] < 103) {
            key[k] = s_key[i] - 87;
        }
        else if (s_key[i] > 64 and s_key[i] < 71) {
            key[k] = s_key[i] - 55;
        }
        if (i != 0) {
            if (s_key[i-1] > 47 and s_key[i - 1] < 58) {
                key[k] += 16*(s_key[i - 1] - 48);
            }
            else if (s_key[i - 1] > 96 and s_key[i - 1] < 103) {
                key[k] += 16 * (s_key[i - 1] - 87);
            }
            else if (s_key[i - 1] > 64 and s_key[i - 1] < 71) {
                key[k] += 16 * (s_key[i - 1] - 55);
            }
        }
        k++;
    }
    key_lenght = k;
    /*for (i = 0; i != key_lenght; i++) {
        cout << int(key[i]) << " ";
    }
    cout << "длина-" << key_lenght << "\n";*/
}

unsigned long long encrypt(unsigned long long byte, unsigned char size) {
    int i;
    switch (enc_type) {
    case(0): return byte; break;
    case(1): {
        for (i = 0; i != size; i++) {
            *((char*)&byte + i) += key[key_pos];
            key_pos++;
            if (key_pos == key_lenght) { key_pos = 0; }
        }
        return byte; break;
    }
    default: {
        for (i = 0; i != size; i++) {
            *((char*)&byte + i) ^= key[key_pos];
            key_pos++;
            if (key_pos == key_lenght) { key_pos = 0; }
        }
        return byte;
    }
    }
}

unsigned long long decrypt(unsigned long long byte, unsigned char size) {
    int i;
    switch (enc_type) {
    case(0): return byte; break;
    case(1): {
        for (i = 0; i != size; i++) {
            *((char*)&byte + i) -= key[key_pos];
            key_pos++;
            if (key_pos == key_lenght) { key_pos = 0; }
        }
        return byte;
    }
    default: {
        for (i = 0; i != size; i++) {
            *((char*)&byte + i) ^= key[key_pos];
            key_pos++;
            if (key_pos == key_lenght) { key_pos = 0; }
        }
        return byte;
    }
    }
}

string get_file_name(unsigned short start_file, unsigned int pointer, short start_info_point=0, unsigned int start_i=0) {
    ifstream file(root + "file" + char(start_file + 48) + ".vup", ios::binary | ios::in);
    file.seekg(pointer, ios::beg);
    unsigned short info_point = start_info_point, next_file, place_point=6 - start_info_point;
    unsigned int i, next_place, place_size;
    char symbol;
    string name = "";
    file.read((char*)&next_place, 4);
    file.read((char*)&next_file, 2);
    file.seekg(6, ios::cur);
    file.read((char*)&place_size, 4);
    if(info_point!=6 and place_size<7-info_point){
        name = get_file_name(next_file, next_place, place_size + info_point);
        return name;
    }
    file.seekg(pointer + 22 - start_info_point, ios::beg);
    for (i=start_i; i != 256 and place_size>place_point; i++) {
        place_point++;
        file.read((char*)&symbol, 1);
        if (symbol == 0) { break; }
        name += symbol;
    }
    if (place_size <= place_point and i != 256) {
        name += get_file_name(next_file, next_place, 6, i);
    }
    return name;
}

unsigned long long get_file_weight(unsigned short next_file, unsigned int next_place) {
    unsigned short byte2, info_point = 0;
    unsigned long long byte8, file_size = 0;
    unsigned int byte4, i, place_size = 0;
    unsigned char byte1, type;
    bool first_file = true;

    while (true) {
        fstream file(root + "file" + char(next_file + 48) + ".vup", ios::binary | ios::out | ios::in);
        file.seekg(next_place, ios::beg);
        file.read((char*)&next_place, 4);
        file.read((char*)&next_file, 2);
        file.seekg(6, ios::cur);
        file.read((char*)&place_size, 4);
        i = 0;
        for (i = i; i < place_size and info_point != 5; i++) {
            if (info_point < 1) {
                file.read((char*)&type, 1);
                info_point = 1;
            }
            else if (info_point < 5) {
                file.read((char*)&byte1, 1);
                file_size += byte1 * pow(256, info_point - 1);
                info_point++;
            }
        }
        if (first_file == true) {
            first_file = false;
            file.read((char*)&next_place, 4);
            file.read((char*)&next_file, 2);
        }
        file.close();
        if (info_point == 5) {
            break;
        }
    }
    return file_size;
}

unsigned short get_file_enc_type(unsigned short next_file, unsigned int next_place) {
    unsigned short byte2, info_point = 0;
    unsigned long long byte8, file_size = 0;
    unsigned int byte4, i, place_size = 0;
    unsigned char byte1, type;
    bool first_file = true;

    while (true) {
        fstream file(root + "file" + char(next_file + 48) + ".vup", ios::binary | ios::out | ios::in);
        file.seekg(next_place, ios::beg);
        file.read((char*)&next_place, 4);
        file.read((char*)&next_file, 2);
        file.seekg(6, ios::cur);
        file.read((char*)&place_size, 4);
        i = 0;
        for (i = i; i < place_size and info_point != 1; i++) {
            if (info_point < 1) {
                file.read((char*)&type, 1);
                info_point = 1;
            }
        }
        if (first_file == true) {
            first_file = false;
            file.read((char*)&next_place, 4);
            file.read((char*)&next_file, 2);
        }
        file.close();
        if (info_point ==1) {
            break;
        }
    }
    return type;
}

void file_create(int num) {
    ofstream file(root + "file" + char(num+48) + ".vup", ios::binary | ios::out);
    cout << "Создан файл " << root + "file" + char(num + 48) + ".vup\n";
    char name[33]{ 'V', 'U', 'P', num%256,num/256,0,0,0,0,0,0,16,0,0,0,255,0,0,0,0,0,0,0,0,240,255,255,255,16,0,0,0,1};
    file.write((char*)&name, 33);
    file.close();
}

unsigned short folder_view(unsigned short start_file, unsigned int pointer, unsigned short files=0) {
    unsigned int byte4;
    unsigned short byte2;
    
    if (!fs::exists(root + "file" + char(start_file + 48) + ".vup")) {file_create(start_file);}
    
    ifstream file(root + "file" + char(start_file + 48) + ".vup", ios::binary | ios::in);

    while (true) {
        file.seekg(pointer, ios::beg);
        file.read((char*)&byte4, 4);
        file.read((char*)&byte2, 2);
        if (byte2 == start_file) {
            if (byte4 == 0) {
                break;
            }
            filelist[files][0] = start_file;
            filelist[files][1] = byte4;
            pointer = byte4;
            files++;
        }
        else {
            files = folder_view(byte2, byte4, files);
            break;
        }
    }
    file.close();
    return files;
}

void header_writing(short start_info_point, short info_point, unsigned int start_place, unsigned short start_file, unsigned int start_file_point, unsigned int file_point, unsigned int previous_place, unsigned short previous_file, unsigned int next_place, unsigned int last_place, unsigned int place_size, unsigned long long result, bool empty_fill_on_end) {
    unsigned int byte4, place_end, place_lenght;
    unsigned short byte2;
    place_end = info_point == -1 ? (start_info_point == -1 ? 0 : 262 - start_info_point) : info_point - start_info_point;
    place_end += file_point - start_file_point + start_place + 16;
    place_lenght = place_end - start_place - 16;
    if (empty_fill_on_end == true) {
        fstream file(root + "file" + char(start_file + 48) + ".vup", ios::binary | ios::out | ios::in);
        if (next_place != 0) {
            file.seekg(next_place+4, ios::beg);
            file.write((char*)&place_end, 4);
            file.seekg(place_end, ios::beg);
            file.write((char*)&next_place, 4);
        }
        else {
            file.seekg(place_end, ios::beg);
            byte4 = 0;
            file.write((char*)&byte4, 4);
        }
        file.write((char*)&byte4, 4);
        file.write((char*)&byte4, 4);
        file.write((char*)&byte4, 4);
        file.write((char*)&byte2, 2);
        file.seekg(11, ios::beg);
        file.write((char*)&place_end, 4);
        file.close();
    }
    if (start_info_point == 0) {
        unsigned short filecount = folder_view(0, 5);
        if (filecount > 0) {
            fstream file(root + "file" + char(filelist[filecount - 1][0] + 48) + ".vup", ios::binary | ios::out | ios::in);
            file.seekg(filelist[filecount - 1][1], ios::beg);
            byte4 = start_place;
            byte2 = start_file;
            file.write((char*)&byte4, 4);
            file.write((char*)&byte2, 2);
            file.close();
            fstream file2(root + "file" + char(start_file + 48) + ".vup", ios::binary | ios::out | ios::in);
            file2.seekg(byte4, ios::beg);
            byte4 = 0;
            byte2 = 0;
            file2.write((char*)&byte4, 4);
            file2.write((char*)&byte2, 2);
            byte4 = filelist[filecount - 1][1];
            byte2 = filelist[filecount - 1][0];
            file2.write((char*)&byte4, 4);
            file2.write((char*)&byte2, 2);
            file2.write((char*)&place_lenght, 4);
            file2.close();
        }
        else {
            fstream file(root + "file" + char(start_file + 48) + ".vup", ios::binary | ios::out | ios::in);
            byte4 = 16;
            byte2 = start_file;
            file.seekg(5, ios::beg);
            file.write((char*)&byte4, 4);
            file.write((char*)&byte2, 2);
            file.seekg(byte4, ios::beg);
            byte4 = 0;
            byte2 = 0;
            file.write((char*)&byte4, 4);
            file.write((char*)&byte2, 2);
            byte4 = 0;
            byte2 = 0;
            file.write((char*)&byte4, 4);
            file.write((char*)&byte2, 2);
            file.write((char*)&place_lenght, 4);
            file.close();
        }
    }
    else{
        fstream file(root + "file" + char(start_file + 48) + ".vup", ios::binary | ios::out | ios::in);
        file.seekg(start_place, ios::beg);
        byte4 = result % 4294967296;
        byte2 = result / 4294967296;
        file.write((char*)&byte4, 4);
        file.write((char*)&byte2, 2);
        byte4 = previous_place;
        byte2 = previous_file;
        file.write((char*)&byte4, 4);
        file.write((char*)&byte2, 2);
        file.write((char*)&place_lenght, 4);
        file.close();
    }
    if (result / 4294967296 != start_file and result!=0 and empty_fill_on_end==false) {
        fstream file(root + "file" + char(start_file + 48) + ".vup", ios::binary | ios::out | ios::in);
        file.seekg(11, ios::beg);
        if (next_place == 0) {
            byte4 = 0;
        }
        else {
            byte4 = next_place;
        }
        file.seekg(11, ios::beg);
        file.write((char*)&byte4, 4);
        file.close();
    }
    else if (result == 0 and empty_fill_on_end == false) {
        fstream file(root + "file" + char(start_file + 48) + ".vup", ios::binary | ios::out | ios::in);
        file.seekg(place_end, ios::beg);
        file.write((char*)&next_place, 4);
        byte4 = 0;
        byte2 = 0;
        file.write((char*)&byte4, 4);
        byte4 = start_place-18+place_size-place_end;
        file.write((char*)&byte4, 4);
        byte4 = 0;
        file.write((char*)&byte4, 4);
        file.write((char*)&byte2, 2);
        file.seekg(11, ios::beg);
        file.write((char*)&place_end, 4);
        file.close();
    }
}

unsigned int file_insert(string path, unsigned short start_file=0, short info_point=0, unsigned long long file_point=0, unsigned int start_place=0, unsigned short previous_file = 0, unsigned int previous_place = 0) {
    string file_name="";
    int i,i2;
    for (i = path.length()-1; path[i] != '/' and path[i] != '\\' and i>2; i--) {}
    for (i++; i != path.length(); i++) {
        if (path[i] < 0) {
            file_name += '#';
        }
        else {
            file_name += path[i];
        }
    }
    /*for (i = 0; i != folder_view(0, 5); i++) {
        if (get_file_name(filelist[i][0], filelist[i][1]) == file_name) {
            while (true) {
                for (i2 = 0; i2 != folder_view(0, 5); i2++) {
                    if (get_file_name(filelist[i][0], filelist[i][1]) == file_name + '(' + char(i2 + 1) + ')') {
                        continue;
                    }
                    break;
                }
            }
            file_name += '(' + char(i2 + 1) + ')';
        }
    }*/
    unsigned long long byte8=0, file_size = fs::file_size(path), start_file_point=file_point, result=0;
    unsigned int byte4=0, next_place, last_place, place_size;
    unsigned short byte2=0;
    unsigned char byte1=0;
    bool empty_fill_on_end = false;
    short start_info_point = info_point;
    
    if (!fs::exists(root + "file" + char(start_file + 48) + ".vup")) { file_create(start_file); }

    fstream file(root + "file" + char(start_file + 48) + ".vup", ios::binary | ios::out | ios::in);

    if (start_place == 0) {
        file.seekg(11, ios::beg);
        file.read((char*)&byte4, 4);
        start_place = byte4;
    }
    if (start_place!=0) {
        file.seekg(start_place, ios::beg);
        file.read((char*)&next_place, 4);
        file.read((char*)&last_place, 4);
        file.read((char*)&place_size, 4);
        byte4 = info_point == -1 ? (start_info_point == -1 ? 0 : 262 - start_info_point) : info_point - start_info_point;
        byte4 += file_point - start_file_point + 16;
        if ((place_size >= byte4) and (place_size - 22 < byte4)) {
            place_size -= 22 - (place_size-byte4);
            empty_fill_on_end = true;
        }
        if (byte4 < place_size and start_info_point==0) { place_size -= 6; }
        file.seekg(4, ios::cur);
        while (info_point!=-1 and info_point-start_info_point < place_size) {
            switch (info_point) {
            case(0): byte1 = enc_type; break;
            case(1): byte1 = file_size%256; break;
            case(2): byte1 = file_size % 65536 / 256; break;
            case(3): byte1 = file_size % 16777216 / 65536; break;
            case(4): byte1 = file_size / 16777216; break;
            case(5): byte1 = 0; break;
            }
            if (info_point >= 6 and info_point <= 261) {
                if (info_point - 6 < file_name.length()) {
                    byte1 = file_name[info_point - 6] % 256;
                }
                else {
                    byte1 = 0;
                }
            }
            file.write((char*)&byte1, 1);
            info_point++;
            if (info_point == 262) { info_point = -1; }
        }
        if (info_point < 262 and info_point!=-1) {
            if (next_place != 0) {
                byte4 = next_place;
                byte2 = start_file;
                file.write((char*)&byte4, 4);
                file.write((char*)&byte2, 2);
            }
            else {
                i = start_file + 1;
                while (true) {
                    if (!fs::exists(root + "file" + char(i + 48) + ".vup")) { file_create(i); }
                    ifstream file2(root + "file" + char(i + 48) + ".vup", ios::binary | ios::in);
                    file2.seekg(11, ios::beg);
                    file2.read((char*)&byte4, 4);
                    file2.close();
                    if (byte4 == 0) {
                        i++;
                    }
                    else { break; }
                }
                byte2 = i;
                file.write((char*)&byte4, 4);
                file.write((char*)&byte2, 2);
            }
            file.close();
            result = file_insert(path, byte2, info_point, file_point, byte4, start_file, start_place);
            header_writing(start_info_point, info_point, start_place, start_file, start_file_point, file_point, previous_place, previous_file, next_place, last_place, place_size, result, empty_fill_on_end);
            return start_place + start_file * 4294967296;
        }
        else {
            ifstream inputing(path, ios::binary | ios::in);
            while (file_point != file_size and file_point - start_file_point < place_size) {
                inputing.seekg(file_point, ios::beg);
                if ((file_point + 8 < file_size) and (file_point + 8 < place_size)) {
                    inputing.read((char*)&byte8, 8);
                    //byte8 = enc_8bit(byte8);
                    byte8 = encrypt(byte8, 8);
                    file.write((char*)&byte8, 8);
                    file_point += 8;
                }
                else if ((file_point + 4 < file_size) and (file_point + 4 < place_size)) {
                    inputing.read((char*)&byte4, 4);
                    //byte4 = enc_4bit(byte4);
                    byte4 = encrypt(byte4, 4);
                    file.write((char*)&byte4, 4);
                    file_point += 4;
                }
                else if ((file_point + 2 < file_size) and (file_point + 2 < place_size)) {
                    inputing.read((char*)&byte2, 2);
                    //byte2 = enc_2bit(byte2);
                    byte2 = encrypt(byte2, 2);
                    file.write((char*)&byte2, 2);
                    file_point += 2;
                }
                else {
                    inputing.read((char*)&byte1, 1);
                    //byte1 = enc_1bit(byte1);
                    byte1 = encrypt(byte1, 1);
                    file.write((char*)&byte1, 1);
                    file_point++;
                }
                if (((file_point+1) * 100 / file_size) / 10 == proc+1){
                    proc = ((file_point + 1) * 100 / file_size) / 10;
                    cout << proc*10 << "% ";
                    if (proc == 100) {
                        cout << "\n";
                    }
                }
            }
            inputing.close();

            if (file_point < file_size) {
                if (next_place != 0) {
                    byte4 = next_place;
                    byte2 = start_file;
                    if (start_info_point == 0) {
                        file.write((char*)&byte4, 4);
                        file.write((char*)&byte2, 2);
                    }
                }
                else {
                    i = start_file+1;
                    while (true) {
                        if (!fs::exists(root + "file" + char(i + 48) + ".vup")) { file_create(i); }
                        ifstream file2(root + "file" + char(i + 48) + ".vup", ios::binary | ios::in);
                        file2.seekg(11, ios::beg);
                        file2.read((char*)&byte4, 4);
                        file2.close();
                        if (byte4 == 0) {
                            i++;
                        }
                        else { break; }
                    }
                    byte2 = i;
                    if (start_info_point == 0) {
                        file.write((char*)&byte4, 4);
                        file.write((char*)&byte2, 2);
                    }
                }
                file.close();
                result = file_insert(path, byte2, info_point, file_point, byte4, start_file, start_place);
                header_writing(start_info_point, info_point, start_place, start_file, start_file_point, file_point, previous_place, previous_file, next_place, last_place, place_size, result, empty_fill_on_end);
                return start_place + start_file*4294967296;
            }
            else {
                file.close();
                header_writing(start_info_point, info_point, start_place, start_file, start_file_point, file_point, previous_place, previous_file, next_place, last_place, place_size, result, empty_fill_on_end);
                return start_place + start_file * 4294967296;
            }
        }
        file.close();
    }
    else {
        file.close();
        result = file_insert(path, start_file + 1, info_point, file_point);
        return result;
    }
}

unsigned int file_read(string path, unsigned short next_file, unsigned int next_place) {
    unsigned short byte2, info_point = 0;
    unsigned long long byte8, file_size = 0;
    unsigned int byte4, i, place_size = 0;
    unsigned char byte1, type;
    bool first_file = true;
    ofstream output(path, ios::binary | ios::out);

    while (true) {
        fstream file(root + "file" + char(next_file + 48) + ".vup", ios::binary | ios::out | ios::in);
        file.seekg(next_place, ios::beg);
        file.read((char*)&next_place, 4);
        file.read((char*)&next_file, 2);
        file.seekg(6, ios::cur);
        file.read((char*)&place_size, 4);
        i = 0;
        for (i = i; i < place_size and info_point != 262; i++) {
            if (info_point < 1) {
                file.read((char*)&type, 1);
                enc_type = type;
                info_point = 1;
            }
            else if (info_point < 5) {
                file.read((char*)&byte1, 1);
                file_size += byte1*pow(256, info_point-1);
                info_point++;
            }
            else if (info_point < 262) {
                file.read((char*)&byte1, 1);
                info_point++;
            }
        }
        if (info_point == 262) {
            while (i < place_size) {
                if (i + 8 < place_size) {
                    file.read((char*)&byte8, 8);
                    byte8 = decrypt(byte8,8);
                    output.write((char*)&byte8, 8);
                    i += 8;
                }
                else if (i + 4 < place_size) {
                    file.read((char*)&byte4, 4);
                    byte4 = decrypt(byte4, 4);
                    output.write((char*)&byte4, 4);
                    i += 4;
                }
                else if (i + 2 < place_size) {
                    file.read((char*)&byte2, 2);
                    byte2 = decrypt(byte2, 2);
                    output.write((char*)&byte2, 2);
                    i += 2;
                }
                else {
                    file.read((char*)&byte1, 1);
                    byte1 = decrypt(byte1, 1);
                    output.write((char*)&byte1, 1);
                    i++;
                }
                /*if (((i + 1) * 100 / file_size) / 10 == proc + 1) {
                    proc = ((file_point + 1) * 100 / file_size) / 10;
                    cout << proc * 10 << "% ";
                    if (proc == 100) {
                        cout << "\n";
                    }
                }*/
            }
        }
        if (file_size - output.tellp() == 0) { break; }
        if (first_file == true) {
            first_file = false;
            file.read((char*)&next_place, 4);
            file.read((char*)&next_file, 2);
        }
        file.close();
    }
    output.close();
    return 1;
}

unsigned int file_delete(unsigned short start_file, unsigned int start_place) {
    unsigned short byte2, info_point = 0, next_file, last_file;
    unsigned long long byte8, file_size = get_file_weight(start_file, start_place);
    unsigned int byte4=0, i, place_size = 0, next_place, last_place, last_empty=0, next_empty=0, empty_size;
    unsigned char byte1;
    bool first_file = true;

    while (true) {
        {fstream file(root + "file" + char(start_file + 48) + ".vup", ios::binary | ios::out | ios::in);
        file.seekg(start_place, ios::beg);
        file.read((char*)&next_place, 4);
        file.read((char*)&next_file, 2);
        file.read((char*)&last_place, 4);
        file.read((char*)&last_file, 2);
        file.read((char*)&place_size, 4);
        file.close(); }
        if (first_file == true) {
            {fstream file(root + "file" + char(last_file + 48) + ".vup", ios::binary | ios::out | ios::in);
            if (last_place != 0) {
                file.seekg(last_place, ios::beg);
            }
            else {
                file.seekg(5, ios::beg);
            }
            file.write((char*)&next_place, 4);
            file.write((char*)&next_file, 2);
            file.close(); }
            if (next_place != 0) {
                fstream file(root + "file" + char(next_file + 48) + ".vup", ios::binary | ios::out | ios::in);
                file.seekg(next_place+6, ios::beg);
                file.write((char*)&last_place, 4);
                file.write((char*)&last_file, 2);
                file.close();
            }
            fstream file(root + "file" + char(start_file + 48) + ".vup", ios::binary | ios::out | ios::in);
            first_file = false;
            if (place_size != file_size + 262) {
                file.seekg(place_size, ios::cur);
                file.read((char*)&next_place, 4);
                file.read((char*)&next_file, 2);
            }
            else {
                next_place = 0;
                next_file = 0;
            }
            file.close();
        }
        fstream file(root + "file" + char(start_file + 48) + ".vup", ios::binary | ios::out | ios::in);
        next_empty = 11;
        while (next_empty < start_place and next_empty != 0) {
            file.seekg(next_empty, ios::beg);
            file.read((char*)&next_empty, 4);
        }
        if (next_empty < start_place) {
            if (uint32_t(file.tellp()) != 15) {
                file.seekg(next_empty + 4, ios::beg);
                file.read((char*)&last_empty, 4);
                file.read((char*)&empty_size, 4);
                if (next_empty + 16 + empty_size == start_place) {
                    file.seekg(-4, ios::cur);
                    empty_size += place_size + 16;
                    file.write((char*)&empty_size, 4);
                }
                else {
                    file.seekg(next_empty, ios::beg);
                    file.write((char*)&start_place, 4);
                    file.seekg(start_place, ios::beg);
                    byte4 = 0;
                    file.write((char*)&byte4, 4);
                    file.write((char*)&last_empty, 4);
                    file.write((char*)&place_size, 4);
                }
            }
            else {
                next_empty = 0;
                last_empty = 0;
                empty_size = 0;
                file.seekg(-4, ios::cur);
                file.write((char*)&start_place, 4);
                file.seekg(start_place, ios::beg);
                file.write((char*)&next_empty, 4);
                file.write((char*)&last_empty, 4);
                file.write((char*)&place_size, 4);
            }
        }
        else{
            file.seekg(next_empty+4, ios::beg);
            file.read((char*)&last_empty, 4);
            if (last_empty != 0) {
                file.seekg(last_empty + 8, ios::beg);
                file.read((char*)&empty_size, 4);
            }
            else {
                last_empty = 0;
                empty_size = 1;
            }
            if (last_empty + 16 + empty_size == start_place) {
                file.seekg(-4, ios::cur);
                empty_size += place_size + 16;
                file.write((char*)&empty_size, 4);
                if (next_empty == last_empty + 16 + empty_size) {
                    file.seekg(next_empty+8, ios::beg);
                    file.read((char*)&byte4, 4);
                    empty_size += 16 + byte4;
                    file.seekg(next_empty, ios::beg);
                    file.read((char*)&byte4, 4);
                    file.seekg(last_empty, ios::beg);
                    file.write((char*)&byte4, 4);
                    if(byte4!=0){
                        file.seekg(byte4+4, ios::beg);
                        file.write((char*)&last_empty, 4);
                    }
                }
            }
            else if (next_empty==start_place+16+place_size) {
                file.seekg(next_empty, ios::beg);
                file.read((char*)&next_empty, 4);
                file.seekg(4, ios::cur);
                file.read((char*)&empty_size, 4);
                if (last_place != 0) {
                    file.seekg(last_empty, ios::beg);
                }
                else {
                    file.seekg(11, ios::beg);
                }
                file.write((char*)&start_place, 4);
                if (next_empty != 0) {
                    file.seekg(next_empty+4, ios::beg);
                    file.write((char*)&start_place, 4);
                }
                file.seekg(start_place, ios::beg);
                file.write((char*)&next_empty, 4);
                file.write((char*)&last_empty, 4);
                empty_size += place_size + 16;
                file.write((char*)&empty_size, 4);
                byte4 = 0;
                file.write((char*)&byte4, 4);
            }
            else {
                if (last_place != 0) {
                    file.seekg(last_empty, ios::beg);
                }
                else {
                    file.seekg(11, ios::beg);
                }
                file.write((char*)&start_place, 4);
                file.seekg(next_empty+4, ios::beg);
                file.write((char*)&start_place, 4);
                file.seekg(start_place, ios::beg);
                file.write((char*)&next_empty, 4);
                file.write((char*)&last_empty, 4);
                file.write((char*)&place_size, 4);
                byte4 = 0;
                file.write((char*)&byte4, 4);
            }
        }
        file.close();
        if (next_place == 0 and next_file == 0) {
            return 1;
        }
        else {
            start_place = next_place;
            start_file = next_file;
        }
    }
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "RUS");

    unsigned int byte4=0, choised;
    unsigned short filecount;
    string path, input_key;
    key_pos = 0;
    enc_type = 0;

    /*for (int k = 0; k != argc; k++) {
        cout << argv[k] << "\n";
    }
    cin >> filecount;*/

    if (argc > 1) {
        root = argv[1];
        folder_view(0, 5);
        switch (argv[2][0] - 48) {
        case(0): {
            enc_type = argv[4][0] - 48;
            if (argv[4][0] - 48 != 0) {
                key_transform(argv[5]);
            }
            return file_insert(argv[3]);
            break;
        }
        case(1): {
            if (argc >= 6) {
                key_transform(argv[5]);
            }
            return file_read(argv[4], filelist[argv[3][0] - 48][0], filelist[argv[3][0] - 48][1]);
            break;
        }
        case(2): {
            return file_delete(filelist[argv[3][0] - 48][0], filelist[argv[3][0] - 48][1]);
            break;
        }
        }
    }

    cout << "Укажите путь к папке с файлами ";
    getline(cin, root);
    //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (root != "") { root += "\\"; }

    while (true) {
        key_pos = 0;
        enc_type = 0;
        
        filecount = folder_view(0, 5);
        if (filecount == 0) { cout << "\nФайлов нет!\n"; }
        else {
            cout << "\nСписок файлов в директории root\n";
            for (int i = 0; i != filecount; i++) {
                cout << i + 1 << ") " << get_file_name(filelist[i][0], filelist[i][1]) << " - " << get_file_weight(filelist[i][0], filelist[i][1]) << " байт\n";
            }
        }
        cout << "\n1. Внедрить файл\n2. Извлечь файл\n3. Удалить файл\n";
        switch (choise(3)) {
        case(0):
        {
            cout << "\nУкажите путь к файлу ";
            cin >> path;
            //getline(cin, path);
            //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "\nВес: " << fs::file_size(path) << "\n";

            //cout << "Выберите шифрование:\n1. Без шифрования\n2. Шифр Виженера\n3. XOR\n4. Шифр простой замены\n";
            //enc_type = choise(4);
            cout << "Выберите шифрование:\n1. Без шифрования\n2. Шифр Виженера\n3. XOR\n";
            enc_type = choise(3);
            if (enc_type != 0) {
                cout << "Введите ключ в 16-ричном виде без пробелов ";
                cin >> input_key;
                key_transform(input_key);
            }

            proc = -1;
            switch (file_insert(path)) {
            case(0): cout << "Во время выполнения операции произошла ошибка...\n";
            default: cout << "Операция прошла успешно!\n";
            }
            break;
        }
        case(1):
        {
            cout << "\nВыберите номер файла в текущей директории ";
            unsigned short num = choise(filecount);
            if (get_file_enc_type(filelist[num][0], filelist[num][1]) != 0) {
                cout << "Файл зашиврован, введите ключ в 16-ричном виде без пробелов.\nЕсли ввести неверный ключ, файл всё равно распакуется, но повреждённым.\n";
                cin >> input_key;
                key_transform(input_key);
            }
            cout << "\nУкажите путь, по которому распакуется файл ";
            cin >> path;
            //getline(cin, path);
            //path = "\"" + path + "\"";
            //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            switch (file_read(path, filelist[num][0], filelist[num][1])) {
            case(0): cout << "Во время выполнения операции произошла ошибка...\n";
            default: cout << "Операция прошла успешно!\n";
            }
            break;
        }
        case(2):
        {
            cout << "\nВыберите номер файла в текущей директории ";
            unsigned short num = choise(filecount);
            switch (file_delete(filelist[num][0], filelist[num][1])) {
            case(0): cout << "Во время выполнения операции произошла ошибка...\n";
            default: cout << "Операция прошла успешно!\n";
            }
            break;
        }
        }
    }
}
