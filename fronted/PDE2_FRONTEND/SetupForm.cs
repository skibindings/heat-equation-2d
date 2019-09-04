using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PDE2_FRONTEND
{
    public partial class SetupForm : Form
    {
        public SetupForm()
        {
            InitializeComponent();
            simulation_number = 1;
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            
        }

        private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog choofdlog = new OpenFileDialog();
            choofdlog.Filter = "All Files (*.*)|*.*";
            choofdlog.FilterIndex = 1;
            choofdlog.Multiselect = false;

            if (choofdlog.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = choofdlog.FileName;        
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            OpenFileDialog choofdlog = new OpenFileDialog();
            choofdlog.Filter = "All Files (*.*)|*.*";
            choofdlog.FilterIndex = 1;
            choofdlog.Multiselect = false;

            if (choofdlog.ShowDialog() == DialogResult.OK)
            {
                textBox2.Text = choofdlog.FileName;
            }
        }

        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void textBox4_TextChanged(object sender, EventArgs e)
        {

        }

        private void button3_Click(object sender, EventArgs e)
        {
            // For the example

            // Use ProcessStartInfo class
            String simulation_path = "simulations/" + simulation_number+"/";
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.CreateNoWindow = false;
            startInfo.UseShellExecute = false;
            startInfo.FileName = "bin/simulation.exe";
            startInfo.WindowStyle = ProcessWindowStyle.Hidden;
            startInfo.Arguments = 
                textBox1.Text + " " + 
                textBox2.Text + " " + 
                textBox3.Text + " " +
                trackBar1.Value + " " + 
                trackBar2.Value + " " + 
                textBox4.Text + " " +
                trackBar6.Value + " " + 
                trackBar5.Value + " " + 
                trackBar3.Value + " " +
                trackBar4.Value + " " +
                textBox5.Text + " " +
                textBox6.Text + " " +
                simulation_path;
            try
            {
                // Start the process with the info we specified.
                // Call WaitForExit and then the using statement will close.
                using (Process exeProcess = Process.Start(startInfo))
                {
                    exeProcess.WaitForExit();
                }
            }
            catch
            {
                // Log error.
            }

            ResultsForm form2 = new ResultsForm(float.Parse(textBox4.Text), simulation_number);
            
            form2.Show();
            simulation_number++;
        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {

        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            label5.Text = ""+trackBar1.Value;
        }

        private void trackBar2_Scroll(object sender, EventArgs e)
        {
            label6.Text = "" + trackBar2.Value;
        }

        private void trackBar6_Scroll(object sender, EventArgs e)
        {
            label7.Text = "" + trackBar6.Value;
        }

        private void trackBar5_Scroll(object sender, EventArgs e)
        {
            label9.Text = "" + trackBar5.Value;
        }

        private void trackBar3_Scroll(object sender, EventArgs e)
        {
            label10.Text = "" + trackBar3.Value;
        }

        private void trackBar4_Scroll(object sender, EventArgs e)
        {
            label8.Text = "" + trackBar4.Value;
        }

        private void label7_Click(object sender, EventArgs e)
        {

        }

        private void label8_Click(object sender, EventArgs e)
        {

        }

        private void label5_Click(object sender, EventArgs e)
        {

        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }

        private void label11_Click(object sender, EventArgs e)
        {

        }

        private void label9_Click(object sender, EventArgs e)
        {

        }

        private void textBox5_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox6_TextChanged(object sender, EventArgs e)
        {

        }

        private void SetupForm_Load(object sender, EventArgs e)
        {

        }

        int simulation_number;
    }
}
