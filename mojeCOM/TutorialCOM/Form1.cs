//wygenerowane
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Runtime.CompilerServices;
using System.Threading;
//moje
using System.IO.Ports;
using System.Windows.Forms.DataVisualization.Charting;

namespace TutorialCOM
{
    public partial class RSDC : Form
    {
        //zmienne użytkownika
        System.IO.Ports.SerialPort port;
        delegate void Delegat1();
        Delegat1 moj_del1;
        public int odzaznaczenie;
        public bool start;
        public int p;
        public int wybor_kierunku;
        public int[] wykres1 = new int[100];
        //tablice do wykresow
        public double[] wykres_polozenia_silnik = new double[2500];
        public double[] czas_polozenie_walu_silnika = new double[2500];

        public double[] wykres_polozenia_masa = new double[2500];
        public double[] czas_polozenie_walu_masa = new double[2500];

        public double[] wykres_predkosc_silnik = new double[1000];
        public double[] czas_predkosc_walu_silnika = new double[1000];

        public double[] wykres_predkosc_masa = new double[4000];
        public double[] czas_predkosc_walu_masa = new double[4000];

        public double[] wykres_prad_silnik = new double[4000];
        public double[] czas_prad_silnik = new double[4000];

        //--------------------------------------------------------
        public double[] wykres_zad_polozenia_silnik = new double[2500];
        public double[] czas_zad_polozenie_walu_silnika = new double[2500];

        public double[] wykres_zad_polozenia_masa = new double[2500];
        public double[] czas_zad_polozenie_walu_masa = new double[2500];

        public double[] wykres_zad_predkosc_silnik = new double[1000];
        public double[] czas_zad_predkosc_walu_silnika = new double[1000];

        public double[] wykres_zad_predkosc_masa = new double[4000];
        public double[] czas_zad_predkosc_walu_masa = new double[4000];

        public double[] wykres_zad_prad_silnik = new double[4000];
        public double[] czas_zad_prad_silnik = new double[4000];
        // obliczanie predkosci
        public double[] polozenie_do_predkosci = new double[400];
        //przyblizanie---------------------------------
        public double[] wykres= new double[600];
        public double[] czas = new double[600];

        public double[] wykres_zadana = new double[600];
        public double[] czas_zadana = new double[600];
        //----------------------
        public int przybliz;
        public int[] sygnal = new int[202];
        public char[] wybor = new char[10];
        public char[] wybor1 = new char[3];
        public string[] zapis_do_pilku = new string[4000];
        public int i = 0;
        public double pomiar_mV, prad_mierzony, polozenie_obrot, predkosc_silnik, predkosc_masa;
        public int cz = 0, napiecie = 0;
        public int polozenie_silnik, polozenie_masa, polozenie_do_predkosci_silnik, polozenie_do_predkosci_masa;
        public int wypelnienie_PWM;
        public int kierunek;
        public char test;
        public string test2, test3, stm, przerwanie;
        public string pomiarS, wartosc, symbol, buffor; 
        public int pomiarInt, pomiarD, pomiarE, pomiarF;
        public int kp1;
        //zmienne do przesylania info do regulatora 
        public float wielkosc_zadana, kp, ti, td;
        public string wielkosc_zadana_wyslij, kp_wyslij, ti_wyslij, td_wyslij;
        //bazowanie czujnika
        public float roznica_do_zera = 2417;


        ///------------------------



        public RSDC()
        {
            InitializeComponent();
            //inicjalizacja zmiennej port z domyślnymi wartościami
            port = new SerialPort();
            //ustawienie timeoutów aby program się nie wieszał
            port.ReadTimeout = 500;
            port.WriteTimeout = 500;

            Opcje.Enter += new EventHandler(Opcje_Enter);
            port.DataReceived += new SerialDataReceivedEventHandler(DataRecievedHandler);
           
            
        }
        private void DataRecievedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            start = true;
            
       
        }





        private void DodajKolorowy(System.Windows.Forms.RichTextBox RichTextBox, string Text, System.Drawing.Color Color)
        {
            var StartIndex = RichTextBox.TextLength;
            RichTextBox.AppendText(Text);
            var EndIndex = RichTextBox.TextLength;
            RichTextBox.Select(StartIndex, EndIndex - StartIndex);
            RichTextBox.SelectionColor = Color;
        }

        void Opcje_Enter(object sender, EventArgs e)
        {
            //aktualizacja list
            this.cbName.Items.Clear();
            this.cbParity.Items.Clear();
            this.cbStop.Items.Clear();
            foreach (String s in SerialPort.GetPortNames()) this.cbName.Items.Add(s);
            foreach (String s in Enum.GetNames(typeof(Parity))) this.cbParity.Items.Add(s);
            foreach (String s in Enum.GetNames(typeof(StopBits))) this.cbStop.Items.Add(s);

            //aktualizacja nazw
            cbName.Text = port.PortName.ToString();
            cbBaud.Text = port.BaudRate.ToString();
            cbData.Text = port.DataBits.ToString();
            cbParity.Text = port.Parity.ToString();
            cbStop.Text = port.StopBits.ToString();
        }


        private void butDomyslne_Click(object sender, EventArgs e)
        {
            this.cbName.Text = "COM4";
            this.cbBaud.Text = "57600";
            this.cbData.Text = "8";
            this.cbParity.Text = "None";
            this.cbStop.Text = "One";
        }

        private void doZera_przycisk_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            {
                roznica_do_zera = 2500 - (float)doZera_prad.Value * 185;
                wielkosc_zadana = roznica_do_zera * 100;
                wartosc = wielkosc_zadana.ToString();
                symbol = "x000";
                wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                port.Write(wielkosc_zadana_wyslij);

                Console.WriteLine(roznica_do_zera);
                Console.WriteLine(wielkosc_zadana_wyslij);

            }
            else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
 
        }


        private void butCancel_Click(object sender, EventArgs e)
        {
            cbName.Text = port.PortName.ToString();
            cbBaud.Text = port.BaudRate.ToString();
            cbData.Text = port.DataBits.ToString();
            cbParity.Text = port.Parity.ToString();
            cbStop.Text = port.StopBits.ToString();
        }

        private void pbStatus_Click(object sender, EventArgs e)
        {
            //jeżeli połączenie jest aktywne to je kończymy, zmieniamy kolor na red i zmieniamy napis
            if (port.IsOpen)
            {
                pbStatus.BackColor = System.Drawing.Color.Red;
                port.Close();
                labStatus.Text = "Brak połączenia";
          
            }
            //w przeciwnym wypadku włączamy połączenie, zmieniamy kolor na zielony i zmieniamy napis
            else
            {
                
                try
                {
                   
                    port.PortName = this.cbName.Text;
                    port.BaudRate = Int32.Parse(this.cbBaud.Text);
                    port.DataBits = Int32.Parse(this.cbData.Text);
                    port.Parity = (Parity)Enum.Parse(typeof(Parity), this.cbParity.Text);
                    port.StopBits = (StopBits)Enum.Parse(typeof(StopBits), this.cbStop.Text);
                    
                    port.Open();
                    
                    pbStatus.BackColor = System.Drawing.Color.Green;
                    labStatus.Text = "Aktywne połączenie:" + "\n Nazwa portu: " + port.PortName.ToString() + "\n Prędkość: " + port.BaudRate.ToString() + "\n Bity danych: " +
                    port.DataBits.ToString() + "\n Bity stopu: " + port.StopBits.ToString() + "\n Parzystość: " + port.Parity.ToString();

                }
               
                catch (Exception exc)
                {
                    MessageBox.Show("Błąd połączenia:\n" + exc.Message);
                }
            }
        }


        private void button_Start_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            {

                wybor_kierunku = kierunek + wypelnienie_PWM;
                Byte[] tosend = { (Byte)wybor_kierunku };
                port.Write(tosend, 0, 1);
            }
            else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
        }

        private void Wylacz_silnik_klik_Click(object sender, EventArgs e)
        {
        }
        //-------Rysowanie wykresu pradu silnika---------------------------------------
        private void TestWykresPradWal_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            {
                wielkosc_zadana = (float)PradZadanyWartosc.Value*1000;
                    if (wielkosc_zadana < 10)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "a00000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 10 & wielkosc_zadana < 100)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "a0000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 100 & wielkosc_zadana < 1000)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "a000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 1000 & wielkosc_zadana < 10000)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "a00000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                port.Write(wielkosc_zadana_wyslij);
                Thread.Sleep(4003);
                for (int i = 0; i <= 3999; i++)
                { 
                    czas_prad_silnik[i] = i*0.05;
                    port.Write("e000000000");
                    pomiarS = port.ReadLine();
                    zapis_do_pilku[i] = pomiarS;
                    Int32.TryParse(pomiarS, out napiecie);
                    wykres_prad_silnik[i] = napiecie*0.001;

                    if (i >= 1000)
                    {
                        wykres_zad_prad_silnik[i] = (float)PradZadanyWartosc.Value;
                    }

                }

                WykresPradWal.Series["Prąd otrzymany"].Points.DataBindXY(czas_prad_silnik, wykres_prad_silnik);
                WykresPradWal.Series["Prąd zadany"].Points.DataBindXY(czas_prad_silnik, wykres_zad_prad_silnik);

            }
            else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
        }
        private void skalowanie_wyk_prad_Click(object sender, EventArgs e)
        {
            przybliz = (int)wartosc_przyblizenia.Value;
            for (int n = 0; n <= 599; n++)
            {
                wykres[n] = wykres_prad_silnik[przybliz + n];
                czas[n] = czas_prad_silnik[n + przybliz];
                wykres_zadana[n] = wykres_zad_prad_silnik[przybliz + n];
                
                
            }

            przybliz_wykre_pradu.Series["Prąd otrzymany"].Points.DataBindXY(czas, wykres);
            przybliz_wykre_pradu.Series["Prąd zadany"].Points.DataBindXY(czas, wykres_zadana);
        }


        //-------Rysowanie wykresu predkosci silnika---------------------------------------
        private void TestWykresPredkoscWal_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            {
                wielkosc_zadana = (int)pr_zad_s.Value;
                Console.WriteLine(wielkosc_zadana);

                    if (wielkosc_zadana < 10)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "j00000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 10 & wielkosc_zadana < 100)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "j0000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 100 & wielkosc_zadana < 1000)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "j000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 1000 & wielkosc_zadana < 10000)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "j00000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }


                port.Write(wielkosc_zadana_wyslij);
                Thread.Sleep(10012);
                
                for (int i = 0; i <= 999; i++)
                {
                    czas_predkosc_walu_silnika[i] = i;
                    port.Write("e000000000");
                    pomiarS = port.ReadLine();
                    Int32.TryParse(pomiarS, out polozenie_do_predkosci_silnik);

                    wykres_predkosc_silnik[i] = (polozenie_do_predkosci_silnik)*0.001;

                    if (i >= 200)
                    {
                        wykres_zad_predkosc_silnik[i] = (float)pr_zad_s.Value;
                    }

                }
                wykres_predkosci_silnik.Series["Prędkość zadana"].Points.DataBindXY(czas_predkosc_walu_silnika, wykres_zad_predkosc_silnik);
                wykres_predkosci_silnik.Series["Prędkość otrzymana"].Points.DataBindXY(czas_predkosc_walu_silnika, wykres_predkosc_silnik);
            }
            else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
        }

        private void Reset_polozenia_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            {
                port.Write("y000000000");
            }
            else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
        }

        private void test_wykres_predkosci_masy_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            {
                wielkosc_zadana = (int)wartosc_pr_zadana_masa.Value;
                Console.WriteLine(wielkosc_zadana);


                    if (wielkosc_zadana < 10)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "o00000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 10 & wielkosc_zadana < 100)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "o0000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 100 & wielkosc_zadana < 1000)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "o000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 1000 & wielkosc_zadana < 10000)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "o00000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                port.Write(wielkosc_zadana_wyslij);
                Thread.Sleep(6002);

                for (int i = 0; i <= 3999; i++)
                {
                    czas_predkosc_walu_masa[i] = i;
                    port.Write("e000000000");
                    pomiarS = port.ReadLine();
                    Int32.TryParse(pomiarS, out polozenie_do_predkosci_silnik);
                    czas_predkosc_walu_masa[i] = i;
                    wykres_predkosc_masa[i] = (polozenie_do_predkosci_silnik) * 0.001;

                    if (i >= 200)
                    {
                        wykres_zad_predkosc_masa[i] = (float)wartosc_pr_zadana_masa.Value;
                    }

                }
                wykres_predkosci_masy.Series["Prędkość zadana"].Points.DataBindXY(czas_predkosc_walu_masa, wykres_zad_predkosc_masa);
                wykres_predkosci_masy.Series["Prędkość otrzymana"].Points.DataBindXY(czas_predkosc_walu_masa, wykres_predkosc_masa);

            }
            else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
        }

        //-------Rysowanie wykresu polozenie silnika---------------------------------------
        private void TestWykresPolozenieWal_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            {
                wielkosc_zadana = (int)polozenie_zadane.Value;
                
                Console.WriteLine(wielkosc_zadana_wyslij);
                    if (wielkosc_zadana < 10)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "f00000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 10 & wielkosc_zadana < 100)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "f0000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 100 & wielkosc_zadana < 1000)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "f000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 1000 & wielkosc_zadana < 10000)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "f00000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                port.Write(wielkosc_zadana_wyslij);
                Thread.Sleep(3002);
                for (int i = 0; i <= 2499; i++)
                {



                    czas_polozenie_walu_silnika[i] = i;
                    port.Write("z000000000");
                    pomiarS = port.ReadLine();
                    zapis_do_pilku[i] = pomiarS;
                    Int32.TryParse(pomiarS, out polozenie_silnik);
                    polozenie_obrot = polozenie_silnik*0.00025;
                    wykres_polozenia_silnik[i] = polozenie_obrot;
                    //Thread.Sleep(1);

                    if (i >= 500)
                    {
                        wykres_zad_polozenia_silnik[i] = (float)polozenie_zadane.Value;
                    }

                }
                WykresPolozenieWalu.Series["Otrzymane położenie"].Points.DataBindXY(czas_polozenie_walu_silnika, wykres_polozenia_silnik);
                WykresPolozenieWalu.Series["Położenie zadane"].Points.DataBindXY(czas_polozenie_walu_silnika, wykres_zad_polozenia_silnik);


            }
            else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
        }
        //-------Rysowanie wykresu polozenie masy---------------------------------------
        private void TestWykresPolozenieMasa_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            {
                wielkosc_zadana = (int)pol_zad_masa.Value;
                Console.WriteLine(wielkosc_zadana_wyslij);

                    if (wielkosc_zadana < 10)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "n00000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 10 & wielkosc_zadana < 100)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "n0000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 100 & wielkosc_zadana < 1000)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "n000000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }
                    else if (wielkosc_zadana >= 1000 & wielkosc_zadana < 10000)
                    {
                        wartosc = wielkosc_zadana.ToString();
                        symbol = "n00000";
                        wielkosc_zadana_wyslij = String.Concat(symbol + wartosc);
                    }

                port.Write(wielkosc_zadana_wyslij);
                Thread.Sleep(3002);
                for (int i = 0; i <= 2499; i++)
                {

                    czas_polozenie_walu_masa[i] = i;
                    port.Write("z000000000");
                    pomiarS = port.ReadLine();
                    zapis_do_pilku[i] = pomiarS;
                    Int32.TryParse(pomiarS, out polozenie_silnik);
                    polozenie_obrot = polozenie_silnik * 0.00025;
                    wykres_polozenia_masa[i] = polozenie_obrot;

                    if (i >= 500)
                    {
                        wykres_zad_polozenia_masa[i] = (float)pol_zad_masa.Value;
                    }

                }
                WykresPolozeniaMasy.Series["Otrzymane położenie"].Points.DataBindXY(czas_polozenie_walu_masa, wykres_polozenia_masa);
                WykresPolozeniaMasy.Series["Położenie zadane"].Points.DataBindXY(czas_polozenie_walu_masa, wykres_zad_polozenia_masa);

            }
            else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
        }
 

        //wysylanie nastaw regulatora polozenia masy------------------------------------------------------------
        private void WyslijNastawyPolM_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            { 
            kp = (float)kpWartoscPolM.Value * 1000;

            ti = (float)TiWartoscPolM.Value * 1000;

            td = (float)TdWartoscPolM.Value * 1000;



            //Dodanie znaków do stringa kp
            if (kp < 10)
            {
                wartosc = kp.ToString();
                symbol = "g00000000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 10 & kp < 100)
            {
                wartosc = kp.ToString();
                symbol = "g0000000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 100 & kp < 1000)
            {
                wartosc = kp.ToString();
                symbol = "g000000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 1000 & kp < 10000)
            {
                wartosc = kp.ToString();
                symbol = "g00000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 10000 & kp < 100000)
            {
                wartosc = kp.ToString();
                symbol = "g0000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 100000 & kp < 1000000)
            {
                wartosc = kp.ToString();
                symbol = "g000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }

            //Dodanie znaków do stringa ti
            if (ti < 10)
            {
                wartosc = ti.ToString();
                symbol = "h00000000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 10 & ti < 100)
            {
                wartosc = ti.ToString();
                symbol = "h0000000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 100 & ti < 1000)
            {
                wartosc = ti.ToString();
                symbol = "h000000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 1000 & ti < 10000)
            {
                wartosc = ti.ToString();
                symbol = "h00000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 10000 & ti < 100000)
            {
                wartosc = ti.ToString();
                symbol = "h0000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 100000 & ti < 1000000)
            {
                wartosc = ti.ToString();
                symbol = "h000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }

            //Dodanie znaków do stringa td
            if (td < 10)
            {
                wartosc = td.ToString();
                symbol = "i00000000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 10 & td < 100)
            {
                wartosc = td.ToString();
                symbol = "i0000000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 100 & td < 1000)
            {
                wartosc = td.ToString();
                symbol = "i000000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 1000 & td < 10000)
            {
                wartosc = td.ToString();
                symbol = "i00000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 10000 & td < 100000)
            {
                wartosc = td.ToString();
                symbol = "i0000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 100000 & td < 1000000)
            {
                wartosc = td.ToString();
                symbol = "i000";
                td_wyslij = String.Concat(symbol + wartosc);
            }


            port.Write(kp_wyslij);
            Thread.Sleep(1);
            port.Write(ti_wyslij);
            Thread.Sleep(1);
            port.Write(td_wyslij);
             }
                else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
            }
        //wysylanie nastaw regulatora polozenia silnika-------------------------------------------------------------
        private void WyslijNastawyPS_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            { 
            kp = (float)kpWartoscPolS.Value*1000;

            ti = (float)TiWartoscPolS.Value * 1000;

            td = (float)TdWartoscPolS.Value * 1000;


            Console.WriteLine(kp);
            Console.WriteLine(ti);
            Console.WriteLine(td);
            //Dodanie znaków do stringa kp
            if (kp < 10)
            {
                wartosc = kp.ToString();
                symbol = "g00000000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 10 & kp < 100)
            {
                wartosc = kp.ToString();
                symbol = "g0000000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 100 & kp < 1000)
            {
                wartosc = kp.ToString();
                symbol = "g000000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 1000 & kp < 10000)
            {
                wartosc = kp.ToString();
                symbol = "g00000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 10000 & kp < 100000)
            {
                wartosc = kp.ToString();
                symbol = "g0000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 100000 & kp < 1000000)
            {
                wartosc = kp.ToString();
                symbol = "g000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }

            //Dodanie znaków do stringa ti
            if (ti < 10)
            {
                wartosc = ti.ToString();
                symbol = "h00000000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 10 & ti < 100)
            {
                wartosc = ti.ToString();
                symbol = "h0000000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 100 & ti < 1000)
            {
                wartosc = ti.ToString();
                symbol = "h000000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 1000 & ti < 10000)
            {
                wartosc = ti.ToString();
                symbol = "h00000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 10000 & ti < 100000)
            {
                wartosc = ti.ToString();
                symbol = "h0000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 100000 & ti < 1000000)
            {
                wartosc = ti.ToString();
                symbol = "h000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }

            //Dodanie znaków do stringa td
            if (td < 10)
            {
                wartosc = td.ToString();
                symbol = "i00000000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 10 & td < 100)
            {
                wartosc = td.ToString();
                symbol = "i0000000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 100 & td < 1000)
            {
                wartosc = td.ToString();
                symbol = "i000000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 1000 & td < 10000)
            {
                wartosc = td.ToString();
                symbol = "i00000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 10000 & td < 100000)
            {
                wartosc = td.ToString();
                symbol = "i0000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 100000 & td < 1000000)
            {
                wartosc = td.ToString();
                symbol = "i000";
                td_wyslij = String.Concat(symbol + wartosc);
            }


            port.Write(kp_wyslij);
            Thread.Sleep(1);
            port.Write(ti_wyslij);
            Thread.Sleep(1);
            port.Write(td_wyslij);
             }
                else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
            }

        //wysylanie nastaw regulatora prad silnik-------------------------------------------------------------
        private void button_wyslij_nastawy_reg_prad_silnik_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            { 
            
            kp = (float)wartosc_Kp_prad_silnik.Value*100;

            ti = (float)wartosc_Ti_prad_silnik.Value*100;

            td = (float)wartosc_Td_prad_silnik.Value*100;


            Console.WriteLine(kp);
            Console.WriteLine(ti);
            Console.WriteLine(td);
            //Dodanie znaków do stringa kp
            if (kp < 10)
            {
                wartosc = kp.ToString();
                symbol = "b00000000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 10 & kp < 100)
            {
                wartosc = kp.ToString();
                symbol = "b0000000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 100 & kp < 1000)
            {
                wartosc = kp.ToString();
                symbol = "b000000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 1000 & kp < 10000)
            {
                wartosc = kp.ToString();
                symbol = "b00000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 10000 & kp < 100000)
            {
                wartosc = kp.ToString();
                symbol = "b0000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }
            else if (kp >= 100000 & kp < 1000000)
            {
                wartosc = kp.ToString();
                symbol = "b000";
                kp_wyslij = String.Concat(symbol + wartosc);
            }


            //Dodanie znaków do stringa ti
            if (ti < 10)
            {
                wartosc = ti.ToString();
                symbol = "c00000000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 10 & ti < 100)
            {
                wartosc = ti.ToString();
                symbol = "c0000000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 100 & ti < 1000)
            {
                wartosc = ti.ToString();
                symbol = "c000000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 1000 & ti < 10000)
            {
                wartosc = ti.ToString();
                symbol = "c00000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 10000 & ti < 100000)
            {
                wartosc = ti.ToString();
                symbol = "c0000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }
            else if (ti >= 100000 & ti < 1000000)
            {
                wartosc = ti.ToString();
                symbol = "c000";
                ti_wyslij = String.Concat(symbol + wartosc);
            }

            //Dodanie znaków do stringa td
            if (td < 10)
            {
                wartosc = td.ToString();
                symbol = "d00000000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 10 & td < 100)
            {
                wartosc = td.ToString();
                symbol = "d0000000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 100 & td < 1000)
            {
                wartosc = td.ToString();
                symbol = "d000000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 1000 & td < 10000)
            {
                wartosc = td.ToString();
                symbol = "d00000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 10000 & td < 100000)
            {
                wartosc = td.ToString();
                symbol = "d0000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            else if (td >= 100000 & td < 1000000)
            {
                wartosc = td.ToString();
                symbol = "d000";
                td_wyslij = String.Concat(symbol + wartosc);
            }
            Console.WriteLine(kp_wyslij);
            Console.WriteLine(ti_wyslij);
            Console.WriteLine(td_wyslij);
            
            port.Write(kp_wyslij);
            Thread.Sleep(1);
            port.Write(ti_wyslij);
            Thread.Sleep(1);
            port.Write(td_wyslij);

            }
            else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
        }
        //wysylanie nastaw regulatora predkosc silnik-------------------------------------------------------------
        private void wyslij_nastawy_PID_pr_s(object sender, EventArgs e)
        {
            if (port.IsOpen)
            {

                kp = (float)kp_pr_s.Value * 1000;

                ti = (float)ti_pr_s.Value * 1000;

                td = (float)td_pr_s.Value * 1000;


                Console.WriteLine(kp);
                Console.WriteLine(ti);
                Console.WriteLine(td);
                //Dodanie znaków do stringa kp
                if (kp < 10)
                {
                    wartosc = kp.ToString();
                    symbol = "k00000000";
                    kp_wyslij = String.Concat(symbol + wartosc);
                }
                else if (kp >= 10 & kp < 100)
                {
                    wartosc = kp.ToString();
                    symbol = "k0000000";
                    kp_wyslij = String.Concat(symbol + wartosc);
                }
                else if (kp >= 100 & kp < 1000)
                {
                    wartosc = kp.ToString();
                    symbol = "k000000";
                    kp_wyslij = String.Concat(symbol + wartosc);
                }
                else if (kp >= 1000 & kp < 10000)
                {
                    wartosc = kp.ToString();
                    symbol = "k00000";
                    kp_wyslij = String.Concat(symbol + wartosc);
                }
                else if (kp >= 10000 & kp < 100000)
                {
                    wartosc = kp.ToString();
                    symbol = "k0000";
                    kp_wyslij = String.Concat(symbol + wartosc);
                }
                else if (kp >= 100000 & kp < 1000000)
                {
                    wartosc = kp.ToString();
                    symbol = "k000";
                    kp_wyslij = String.Concat(symbol + wartosc);
                }

                //Dodanie znaków do stringa ti
                if (ti < 10)
                {
                    wartosc = ti.ToString();
                    symbol = "l00000000";
                    ti_wyslij = String.Concat(symbol + wartosc);
                }
                else if (ti >= 10 & ti < 100)
                {
                    wartosc = ti.ToString();
                    symbol = "l0000000";
                    ti_wyslij = String.Concat(symbol + wartosc);
                }
                else if (ti >= 100 & ti < 1000)
                {
                    wartosc = ti.ToString();
                    symbol = "l000000";
                    ti_wyslij = String.Concat(symbol + wartosc);
                }
                else if (ti >= 1000 & ti < 10000)
                {
                    wartosc = ti.ToString();
                    symbol = "l00000";
                    ti_wyslij = String.Concat(symbol + wartosc);
                }
                else if (ti >= 10000 & ti < 100000)
                {
                    wartosc = ti.ToString();
                    symbol = "l0000";
                    ti_wyslij = String.Concat(symbol + wartosc);
                }
                else if (ti >= 100000 & ti < 1000000)
                {
                    wartosc = ti.ToString();
                    symbol = "l000";
                    ti_wyslij = String.Concat(symbol + wartosc);
                }

                //Dodanie znaków do stringa td
                if (td < 10)
                {
                    wartosc = td.ToString();
                    symbol = "m00000000";
                    td_wyslij = String.Concat(symbol + wartosc);
                }
                else if (td >= 10 & td < 100)
                {
                    wartosc = td.ToString();
                    symbol = "m0000000";
                    td_wyslij = String.Concat(symbol + wartosc);
                }
                else if (td >= 100 & td < 1000)
                {
                    wartosc = td.ToString();
                    symbol = "m000000";
                    td_wyslij = String.Concat(symbol + wartosc);
                }
                else if (td >= 1000 & td < 10000)
                {
                    wartosc = td.ToString();
                    symbol = "m00000";
                    td_wyslij = String.Concat(symbol + wartosc);
                }
                else if (td >= 10000 & td < 100000)
                {
                    wartosc = td.ToString();
                    symbol = "m0000";
                    td_wyslij = String.Concat(symbol + wartosc);
                }
                else if (td >= 100000 & td < 1000000)
                {
                    wartosc = td.ToString();
                    symbol = "m000";
                    td_wyslij = String.Concat(symbol + wartosc);
                }

                port.Write(kp_wyslij);
                Thread.Sleep(1);
                port.Write(ti_wyslij);
                Thread.Sleep(1);
                port.Write(td_wyslij);

            }
            else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
        }
        private void wyslij_nastawy_pid_pr_m_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            {

                kp = (float)wartosc_kp_pr_m.Value * 1000;

                ti = (float)wartosc_ti_pr_m.Value * 1000;

                td = (float)wartosc_td_pr_m.Value * 1000;


                Console.WriteLine(kp);
                Console.WriteLine(ti);
                Console.WriteLine(td);
                //Dodanie znaków do stringa kp
                if (kp < 10)
                {
                    wartosc = kp.ToString();
                    symbol = "k00000000";
                    kp_wyslij = String.Concat(symbol + wartosc);
                }
                else if (kp >= 10 & kp < 100)
                {
                    wartosc = kp.ToString();
                    symbol = "k0000000";
                    kp_wyslij = String.Concat(symbol + wartosc);
                }
                else if (kp >= 100 & kp < 1000)
                {
                    wartosc = kp.ToString();
                    symbol = "k000000";
                    kp_wyslij = String.Concat(symbol + wartosc);
                }
                else if (kp >= 1000 & kp < 10000)
                {
                    wartosc = kp.ToString();
                    symbol = "k00000";
                    kp_wyslij = String.Concat(symbol + wartosc);
                }
                else if (kp >= 10000 & kp < 100000)
                {
                    wartosc = kp.ToString();
                    symbol = "k0000";
                    kp_wyslij = String.Concat(symbol + wartosc);
                }
                else if (kp >= 100000 & kp < 1000000)
                {
                    wartosc = kp.ToString();
                    symbol = "k000";
                    kp_wyslij = String.Concat(symbol + wartosc);
                }

                //Dodanie znaków do stringa ti
                if (ti < 10)
                {
                    wartosc = ti.ToString();
                    symbol = "l00000000";
                    ti_wyslij = String.Concat(symbol + wartosc);
                }
                else if (ti >= 10 & ti < 100)
                {
                    wartosc = ti.ToString();
                    symbol = "l0000000";
                    ti_wyslij = String.Concat(symbol + wartosc);
                }
                else if (ti >= 100 & ti < 1000)
                {
                    wartosc = ti.ToString();
                    symbol = "l000000";
                    ti_wyslij = String.Concat(symbol + wartosc);
                }
                else if (ti >= 1000 & ti < 10000)
                {
                    wartosc = ti.ToString();
                    symbol = "l00000";
                    ti_wyslij = String.Concat(symbol + wartosc);
                }
                else if (ti >= 10000 & ti < 100000)
                {
                    wartosc = ti.ToString();
                    symbol = "l0000";
                    ti_wyslij = String.Concat(symbol + wartosc);
                }
                else if (ti >= 100000 & ti < 1000000)
                {
                    wartosc = ti.ToString();
                    symbol = "l000";
                    ti_wyslij = String.Concat(symbol + wartosc);
                }

                //Dodanie znaków do stringa td
                if (td < 10)
                {
                    wartosc = td.ToString();
                    symbol = "m00000000";
                    td_wyslij = String.Concat(symbol + wartosc);
                }
                else if (td >= 10 & td < 100)
                {
                    wartosc = td.ToString();
                    symbol = "m0000000";
                    td_wyslij = String.Concat(symbol + wartosc);
                }
                else if (td >= 100 & td < 1000)
                {
                    wartosc = td.ToString();
                    symbol = "m000000";
                    td_wyslij = String.Concat(symbol + wartosc);
                }
                else if (td >= 1000 & td < 10000)
                {
                    wartosc = td.ToString();
                    symbol = "m00000";
                    td_wyslij = String.Concat(symbol + wartosc);
                }
                else if (td >= 10000 & td < 100000)
                {
                    wartosc = td.ToString();
                    symbol = "m0000";
                    td_wyslij = String.Concat(symbol + wartosc);
                }
                else if (td >= 100000 & td < 1000000)
                {
                    wartosc = td.ToString();
                    symbol = "m000";
                    td_wyslij = String.Concat(symbol + wartosc);
                }

                port.Write(kp_wyslij);
                Thread.Sleep(1);
                port.Write(ti_wyslij);
                Thread.Sleep(1);
                port.Write(td_wyslij);

            }
            else System.Windows.Forms.MessageBox.Show("Brak połączenia z urządzeniem");
        }

        private void WyczyscWykresPradWalu_Click(object sender, EventArgs e)
        {
            WykresPradWal.Series["Prąd otrzymany"].Points.Clear();
        }
        private void WyczyscWykresPolozeniaMasa_Click(object sender, EventArgs e)
        {
            WykresPolozeniaMasy.Series["Otrzymane położenie"].Points.Clear();
        }
        private void WyczyscWykresPolozeniaWalu_Click(object sender, EventArgs e)
        {
            WykresPolozenieWalu.Series["Otrzymane położenie"].Points.Clear();
        }
        private void wyczysc_wykres_predkosci_silnik_Click(object sender, EventArgs e)
        {
            wykres_predkosci_masy.Series["Prędkość otrzymana"].Points.Clear();
        }

        private void WyczyscWykresPredkosciWalu_Click(object sender, EventArgs e)
        {
            wykres_predkosci_silnik.Series["Prędkość otrzymana"].Points.Clear();
        }

        private void RegulacjaPredkosci_Click(object sender, EventArgs e)
        {

        }
        private void numericUpDown7_ValueChanged(object sender, EventArgs e)
        {

        }




        private void numericUpDown8_ValueChanged(object sender, EventArgs e)
        {

        }
        //Wyświetlanie wykresu prądu na wale




    }
}
