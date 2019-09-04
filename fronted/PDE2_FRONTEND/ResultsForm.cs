using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PDE2_FRONTEND
{
    public partial class ResultsForm : Form
    {
        public ResultsForm(float exp_duration, int simulation_number_)
        {
            InitializeComponent();
            current_image_number = 0;
            simulation_number = simulation_number_;

            int number = trackBar1.Value;
            pictureBox1.Image = Image.FromFile("simulations/" + simulation_number + "/img_" + number + ".bmp");
            current_image_number = number;

            duration = exp_duration;
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            int number = trackBar1.Value;
            if (current_image_number != number)
            {
                pictureBox1.Image = Image.FromFile("simulations/"+simulation_number+"/img_" + number + ".bmp");
                current_image_number = number;
            }

            float time = ((float)trackBar1.Value / 99.0f) * duration;
            label1.Text = "Время : " + String.Format("{0:0.00}",time) + " c";
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        int current_image_number;
        float duration;
        int simulation_number;

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }

        private void ResultsForm_Load(object sender, EventArgs e)
        {

        }
    }
}
