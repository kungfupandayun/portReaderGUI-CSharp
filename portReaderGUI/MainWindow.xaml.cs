using System;
using System.Windows;
using System.IO.Ports;
using System.IO;
using System.Threading;
using System.Globalization;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace portReaderGUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        [DllImport("C_DLL_with_Csharp.dll", EntryPoint = "DoSomethingInC")]

        String inputTxt, outputTxt,glitchMmtTxt, outputFolderTxt;
        Byte[] plainTxtBytes;
        Byte[][] jaggedArray = new Byte[16][];
        Byte[] encBytes = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        string correctAnswer = "69C4E0D86A7B0430D8CDB78070B4C55A";


        public MainWindow()
        {

            InitializeComponent();
        }

        void button_Click(object sender, RoutedEventArgs e)
        {
            // Show message box when button is clicked.
            //MessageBox.Show("Encryption Started!");
            inputTxt = txtBoxInput.Text;
            glitchMmtTxt = txtGmmt.Text;
            outputFolderTxt = txtOutputFolder.Text;

            plainTxtBytes = new Byte[inputTxt.Length / 2];
            
            
            NumberStyles styles = NumberStyles.HexNumber;
            for (int i = 0; i < plainTxtBytes.Length; i++)
            {
                int j = i*2;
                byte.TryParse(inputTxt.Substring(j,2), styles, null as IFormatProvider, out plainTxtBytes[i]);
            }
            
            
            

            SerialPort port = new SerialPort("COM8", 9600);

            for(int i=0;i<16;i++){
                jaggedArray[i]= new Byte[1];
                jaggedArray[i] =new Byte[]{ plainTxtBytes[i]};
            }



            try
            {
                port.Open();

                for (int j = plainTxtBytes.Length - 1; j >= 0; j--)
                {
                    port.Write(jaggedArray[j], 0, 1);
                    Thread.Sleep(100);
                }

                port.Read(encBytes, 0, plainTxtBytes.Length);
                outputTxt = BitConverter.ToString(encBytes).Replace("-", "");
                String[] doc = { "aes : " + outputTxt, "Iteration #" + glitchMmtTxt, "anr_spareout : 000000" };

                File.AppendAllLines(path: @outputFolderTxt, doc);
                port.Close();
            }
            catch (IOException ex)
            {
                outputTxt = "Port not open / File not found";
            }


            txtBlockOutput.Text = outputTxt;

            //Show if the encrypted answer is correct
            txtCorrectOrNot.Visibility = Visibility.Visible;
            if (outputTxt.Equals(correctAnswer))
            {
                txtCorrectOrNot.Text = "Correct";
            }
            else
            {
                txtCorrectOrNot.Text = "False";
            }
                    
                    
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            txtBlockOutput.Text = "";
            txtCorrectOrNot.Visibility = Visibility.Hidden;

        }

    }
}
