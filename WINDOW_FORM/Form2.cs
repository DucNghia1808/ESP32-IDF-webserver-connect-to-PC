using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace tbnv
{
    public partial class Form2 : Form
    {
        public Form2()
        {
            InitializeComponent();
            this.Text = "LOGIN DASHBOARD";
        }

        private void CANCEL_Click(object sender, EventArgs e)
        {
            DialogResult answer = MessageBox.Show("Do you want to exit the program ?", "Question", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (answer == DialogResult.No)
            {
                
            }
            else
            {
                this.Close();
            }
        }
        string usernname = "admin";
        string pass = "nghia";
        private void LOGGIN_Click(object sender, EventArgs e)
        {
            if (string.Compare(username.Text, usernname) == 0 && string.Compare(password.Text, pass) == 0)
            {
                MessageBox.Show("Login successfully.", "Infomation", MessageBoxButtons.OK, MessageBoxIcon.Information);
           
                this.Hide();
                Form1 form1_moi = new Form1();
                form1_moi.ShowDialog();
            }
            else
            {
                username.Text = "";
                password.Text = "";
                MessageBox.Show("Please check your ID and PassWord !!", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label3_Click(object sender, EventArgs e)
        {

        }
    }
}
