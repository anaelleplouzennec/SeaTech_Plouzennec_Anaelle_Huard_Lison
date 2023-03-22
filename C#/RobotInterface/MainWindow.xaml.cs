using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using ExtendedSerialPort;
using System.Windows.Threading;
namespace RobotInterface
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        ReliableSerialPort serialPort1;
        DispatcherTimer timerAffichage;
        Robot robot;
        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM4", 115200, System.IO.Ports.Parity.None, 8, System.IO.Ports.StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            robot = new Robot();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();

        }


        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            //if (robot.receivedText != "")
            //{
            //    textBoxReception.Text += robot.receivedText;
            //    robot.receivedText = "";
            //}
            while (robot.byteListReceived.Count > 0)
            {
                var c = robot.byteListReceived.Dequeue();
                //textBoxReception.Text += "0x" + c.ToString("X2") + " ";
                DecodeMessage(c);
            }
            // ToString()  donne en décimal
            // ToString("X")  donne en héxadécimal
            // ToString("X2")  donne en héxadécimal le numéro de l'octet+le caractère sur 2 bits
            // ToString("X4")  donne en décimal le numéro de l'octet+le caractère sur 4 bits
        }

        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            //for (int i = 0;i< e.Data.Length;i++)
            foreach (byte value in e.Data)
            {
                robot.byteListReceived.Enqueue(value);
            }
        }

        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            if (buttonEnvoyer.Background == Brushes.RoyalBlue)
            {
                buttonEnvoyer.Background = Brushes.Beige;
            }
            else
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
            }
            SendMessage();
        }

        private void SendMessage()
        {
            //textBoxReception.Text += "Reçu : " + textBoxEmission.Text + "\n";
            serialPort1.WriteLine(textBoxEmission.Text);
            textBoxEmission.Text = "";


        }

        private void TextBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            textBoxReception.Text = "";

        }

        private void Test_Click(object sender, RoutedEventArgs e)
        {
            string s = "Bonjour";
            byte[] array = Encoding.ASCII.GetBytes(s);
            // UartEncodeAndSendMessage(0x0080, array.Length, array);

            array = new byte[] { 1, 0 };
            array[0] = 1;
            array[1] = 1;
            UartEncodeAndSendMessage(0x0020, array.Length, array);
            array[0] = 2;
            array[1] = 0;
            UartEncodeAndSendMessage(0x0020, array.Length, array);
            array[0] = 3;
            array[1] = 1;
            UartEncodeAndSendMessage(0x0020, array.Length, array);

            array = new byte[3];
            array[0] = 1;
            array[1] = 3;
            array[2] = 0;
            UartEncodeAndSendMessage(0x0030, array.Length, array);
            array[0] = 2;
            array[1] = 2;
            array[2] = 0;
            UartEncodeAndSendMessage(0x0030, array.Length, array);
            array[0] = 3;
            array[1] = 5;
            array[2] = 4;
            UartEncodeAndSendMessage(0x0030, array.Length, array);

            array = new byte[3];
            array[0] = 1;
            array[1] = 3;
            array[2] = 5;
            UartEncodeAndSendMessage(0x0040, array.Length, array);
            array[0] = 2;
            array[1] = 2;
            array[2] = 5;
            UartEncodeAndSendMessage(0x0040, array.Length, array);

            if (buttonEnvoyer.Background == Brushes.RoyalBlue)
            {
                buttonEnvoyer.Background = Brushes.Beige;
            }
            else
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
            }
        }
        byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte checksum = 0;
            checksum ^= 0xFE;
            checksum ^= (byte)(msgFunction >> 8);
            checksum ^= (byte)(msgFunction >> 0);
            checksum ^= (byte)(msgPayloadLength >> 8);
            checksum ^= (byte)(msgPayloadLength >> 0);
            for (int i = 0; i < msgPayloadLength; i++)
            {
                checksum ^= msgPayload[i];
            }
            return checksum;
        }
        void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte[] byteList = new byte[msgPayloadLength + 6];

            int pos = 0;

            byteList[pos++] = 0xFE;
            byteList[pos++] = (byte)(msgFunction >> 8);
            byteList[pos++] = (byte)(msgFunction >> 0);
            byteList[pos++] = (byte)(msgPayloadLength >> 8);
            byteList[pos++] = (byte)(msgPayloadLength >> 0);

            for (int i = 0; i < msgPayloadLength; i++)
            {
                byteList[pos++] = msgPayload[i];
            }
            byteList[pos++] = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

            serialPort1.Write(byteList, 0, pos);

        }
        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }
        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;
        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                    }

                    break;
                case StateReception.FunctionMSB:
                    msgDecodedFunction = (c << 8);
                    rcvState = StateReception.FunctionLSB;
                    break;
                case StateReception.FunctionLSB:
                    msgDecodedFunction += (c << 0);
                    rcvState = StateReception.PayloadLengthMSB;
                    break;

                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = (c << 8);
                    rcvState = StateReception.PayloadLengthLSB;
                    break;

                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength += (c << 0);

                    if (msgDecodedPayloadLength == 0)
                    {
                        rcvState = StateReception.CheckSum;
                    }
                    else if (msgDecodedPayloadLength > 1024)
                    {
                        rcvState = StateReception.Waiting;
                    }
                    else
                    {
                        msgDecodedPayload = new byte[msgDecodedPayloadLength];
                        msgDecodedPayloadIndex = 0;
                        rcvState = StateReception.Payload;
                    }
                    
                    break;

                case StateReception.Payload:
                    msgDecodedPayload[msgDecodedPayloadIndex] = c;
                    msgDecodedPayloadIndex++;
                    if(msgDecodedPayloadIndex>= msgDecodedPayloadLength)
                        rcvState = StateReception.CheckSum;
                    break;                    

                case StateReception.CheckSum:
                    byte calculatedChecksum;
                    byte receivedChecksum = c;
                    calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    if (calculatedChecksum == receivedChecksum)
                    {

                        //Success, on a un message valide
                        textBoxReception.Text += "ca marche ";
                        ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    }
                    
                    else
                    {
                        textBoxReception.Text += "ca marche pas du tout ";
                    }
                    rcvState = StateReception.Waiting;

                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }
        void ProcessDecodedMessage(int msgFunction,int msgPayloadLength, byte[] msgPayload)
        {
            if (msgFunction == 0x0020)
            {
                if (msgPayload[0] == 1)
                {
                    if (msgPayload[1] == 1)
                    {
                        LED_1.IsChecked = true;
                    }
                    else
                    {
                        LED_1.IsChecked = false;
                    }
                }
                else if (msgPayload[0] == 2)
                {
                    if (msgPayload[1] == 1)
                    {
                        LED_2.IsChecked = true;
                    }
                    else
                    {
                        LED_2.IsChecked = false;
                    }
                }
                else if (msgPayload[0] == 3)
                {
                    if (msgPayload[1] == 1)
                    {
                        LED_3.IsChecked = true;
                    }
                    else
                    {
                        LED_3.IsChecked = false;
                    }
                }
            }
            else if (msgFunction == 0x0030)
            {
                if (msgPayload[0]==1)
                {
                    Télémètres_IR.Content = " IR Gauche : " + msgPayload[1] + msgPayload[2] + "cm\n" ;
                }
                else if (msgPayload[0] == 2)

                {
                    Télémètres_IR.Content += " IR Centre : " + msgPayload[1] + msgPayload[2] + "cm\n";
                }
                else if (msgPayload[0] == 3)
                {
                    Télémètres_IR.Content += " IR Droit : " + msgPayload[1] + msgPayload[2] + "cm\n";
                }
            }
            else if (msgFunction == 0x0040)
            {
                if (msgPayload[0] == 1)
                {
                    Moteurs.Content = "Vitesse Gauche : " + msgPayload[1] + msgPayload[2] + "%\n";
                }
                else if (msgPayload[0] == 2)

                {
                    Moteurs.Content += "Vitesse Droit : " + msgPayload[1] + msgPayload[2] + "%\n";
                }
            }
        }
    }
}