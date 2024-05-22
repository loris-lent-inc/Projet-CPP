using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing.Imaging;

using System.Runtime.InteropServices;
using libImage;
//using static System.Net.Mime.MediaTypeNames;
using static System.Windows.Forms.AxHost;
using System.Threading;
using System.IO;

namespace seuilAuto
{
    public partial class Form1 : Form
    {
        double moyenne = 0, mediane = 0, currentScore = 0;
        bool run = false;
        List<Image> images;
        List<Image> imagesTraitees;
        int position = 0;

        public enum State
        {
            INIT,
            READY,
            RUN,
            RUN_STOP
        }

        private State currentState = State.INIT;

        private Thread t1;

        public Form1()
        {
            InitializeComponent();
            processState(State.INIT);
        }



        private void button1_Click(object sender, EventArgs e)
        {
            images = LoadBmpImages();
            //Bitmap bmp = new Bitmap("C:\\Users\\loris\\Downloads\\images projet\\Source Images - bmp\\Test.bmp");
            //ClImage Img = new ClImage();

            //unsafe
            //{
            //    BitmapData bmpData = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
            //    Img.objetLibDataImgPtr(1, bmpData.Scan0, bmpData.Stride, bmp.Height, bmp.Width);
            //    // 1 champ texte retour C++, le seuil auto
            //    bmp.UnlockBits(bmpData);
            //}

            //images = new List<Image>();
            //images.Add(bmp);
            //images.Add(bmp);
            //images.Add(bmp);
            //images.Add(bmp);
            loadFirst();
            processState(State.READY);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            processState(State.RUN);
            traitement();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            processState(State.RUN_STOP);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            // bouton pour sauvegarder l'image (laquelle ?)
            SaveFileDialog saveFileDialog1 = new SaveFileDialog();
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {

            }
        }

        private void loadFirst()
        {
            if (images.Count == 0)
            {
                return;
            }
            position = 0;
            pictureBoxPRE.Image = images[position];
            labelNumero.Text = (position + 1) + "/" + images.Count;
        }

        public List<Image> LoadBmpImages()
        {
            List<Image> images = new List<Image>();
            FolderBrowserDialog folderBrowserDialog1 = new FolderBrowserDialog();
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                string[] files = Directory.GetFiles(folderBrowserDialog1.SelectedPath, "*.bmp");
                if (files.Length == 0)
                {
                    MessageBox.Show("Aucune image BMP trouvée dans le dossier sélectionné.");
                    return images;
                }

                foreach (string file in files)
                {
                    Image image = Image.FromFile(file);
                    images.Add(image);
                }
            }
            return images;
        }

        private void traitement()
        {
            PictureBox[] preBoxes = new PictureBox[]
            {
                pictureBoxPRE1, pictureBoxPRE2, pictureBoxPRE3, pictureBoxPRE4, pictureBoxPRE5, pictureBoxPRE6, pictureBoxPRE7, pictureBoxPRE8, pictureBoxPRE9, pictureBoxPRE10

            };
            PictureBox[] postBoxes = new PictureBox[]
            {
                pictureBoxPOST1, pictureBoxPOST2, pictureBoxPOST3, pictureBoxPOST4, pictureBoxPOST5, pictureBoxPOST6, pictureBoxPOST7, pictureBoxPOST8, pictureBoxPOST9, pictureBoxPOST10

            };

            while (currentState == State.RUN)
            {
                goToNext(preBoxes, postBoxes);
                Application.DoEvents();
                Thread.Sleep(10);
                // MessageBox.Show("OK");

            }

        }

        private void goToNext(PictureBox[] preBoxes, PictureBox[] postBoxes)
        {
            Image old = pictureBoxPRE.Image = images[position];
            Bitmap BMP = new Bitmap(old);
            ClImage Img = new ClImage();
            unsafe
            {
                BitmapData bmpData = BMP.LockBits(new Rectangle(0, 0, BMP.Width, BMP.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
                Img.objetLibDataImgPtr(1, bmpData.Scan0, bmpData.Stride, BMP.Height, BMP.Width);
                // 1 champ texte retour C++, le seuil auto
                BMP.UnlockBits(bmpData);
            }

            pictureBoxPOST.Image = BMP;

            labelNumero.Text = (position + 1) + "/" + images.Count;
            

            // Mise à jour des images précédentes
            for (int i = preBoxes.Length - 1; i > 0; i--)
            {
                if (preBoxes[i - 1].Image != null)
                {
                    preBoxes[i].Image = preBoxes[i - 1].Image;
                }

            }
            preBoxes[0].Image = pictureBoxPRE.Image;

            for (int i = postBoxes.Length - 1; i > 0; i--)
            {
                if (postBoxes[i - 1].Image != null)
                {
                    postBoxes[i].Image = postBoxes[i - 1].Image;
                }

            }
            postBoxes[0].Image = pictureBoxPOST.Image;

            position++;
            if (position >= images.Count)
            {
                position = 0;
                processState(State.READY);
                //loadFirst();
                return;
            }
        }

        private void processState(State newState)
        {
            currentState = newState;
            switch (newState)
            {
                case State.INIT:
                    statePanel.BackColor = Color.LightGray;
                    stateLabel.Text = "Initialisé";
                    button1.Enabled = true;
                    button2.Enabled = false;
                    button3.Enabled = false;
                    button4.Enabled = false;
                    break;
                case State.READY:
                    statePanel.BackColor = Color.LightGreen;
                    stateLabel.Text = "Prêt";
                    button1.Enabled = false;
                    button2.Enabled = true;
                    button3.Enabled = false;
                    button4.Enabled = false;
                    break;
                case State.RUN:
                    statePanel.BackColor = Color.Green;
                    stateLabel.Text = "Lancé";
                    button1.Enabled = false;
                    button2.Enabled = false;
                    button3.Enabled = true;
                    button4.Enabled = false;
                    break;
                case State.RUN_STOP:
                    statePanel.BackColor = Color.Orange;
                    stateLabel.Text = "Pause";
                    button1.Enabled = false;
                    button2.Enabled = true;
                    button3.Enabled = false;
                    button4.Enabled = true;
                    break;
            }
        }
    }
}
