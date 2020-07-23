// ====================================================
//                PARSING DATA Serial1 (string & int)
// ====================================================
 
String dataIn;
String dt[10];
int i;
boolean parsing=false;
 
void setup() 
{
    Serial1.begin(115200);
    dataIn="";
}
 
void loop() 
{
     if(Serial1.available()>0)
     {
           char inChar = (char)Serial1.read();
           dataIn += inChar;
           if (inChar == '\n') {parsing = true;}
     }
     if(parsing)
     {
           parsingData();
           parsing=false;
           dataIn="";
     }
}
 
void parsingData()
{
      int j=0;
      //kirim data yang telah diterima sebelumnya
      Serial1.print("data masuk : ");
      Serial1.print(dataIn);
      Serial1.print("\n");
      //inisialisasi variabel, (reset isi variabel)
      dt[j]="";
      //proses parsing data
      for(i=1;i<dataIn.length();i++)
      {
             //pengecekan tiap karakter dengan karakter (#) dan (,)
             if ((dataIn[i] == '#') || (dataIn[i] == ','))
             {
                   //increment variabel j, digunakan untuk merubah index array penampung
                   j++;
                  dt[j]=""; //inisialisasi variabel array dt[j]
             }
             else
             {
                  //proses tampung data saat pengecekan karakter selesai.
                  dt[j] = dt[j] + dataIn[i];
             }
      }
      //kirim data hasil parsing
      Serial1.print("data 1 : ");
      Serial1.print(dt[0]);
      Serial1.print("\n");
      Serial1.print("data 2 : ");
      Serial1.print(dt[1].toInt());
      Serial1.print("\n");
      Serial1.print("data 3 : ");
      Serial1.print(dt[2].toInt());
      Serial1.print("\n");
      Serial1.print("data 4 : ");
      Serial1.print(dt[3].toInt());
      Serial1.print("\n");
      Serial1.print("data 5 : ");
      Serial1.print(dt[4].toInt());
      Serial1.print("\n\n");
}
