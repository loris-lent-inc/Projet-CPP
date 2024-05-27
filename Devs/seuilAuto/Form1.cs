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
using System.Runtime.InteropServices.ComTypes;

namespace seuilAuto
{
    public partial class Form1 : Form
    {
        int moyenne = 0, mediane = 0, currentScore = 0, somme = 0;
        bool run = false;
        List<Image> imagesPost = new List<Image>();
        List<Image> imagesTraitees = new List<Image>();
        List<String> titres = new List<String>();
        
        List<Tuple<Image, Image>> images;
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
            // dossier de destination
            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();
            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                // chemin du dossier sélectionné
                string selectedPath = folderBrowserDialog.SelectedPath;

                // chemins des dossiers pour les images PRE et POST
                string preFilePath = Path.Combine(selectedPath, "PreImages");
                string postFilePath = Path.Combine(selectedPath, "PostImages");

                saveImage(imagesPost,imagesTraitees,preFilePath,postFilePath);

                MessageBox.Show("Images sauvegardées avec succès!");
             }
            
        }

        private void loadFirst()
        {
            if (images.Count == 0)
            {
                return;
            }
            position = 0;
            pictureBoxPRE.Image = images[position].Item1;
            labelNumero.Text = (position + 1) + "/" + images.Count;
        }

        public List<Tuple<Image, Image>> LoadBmpImages()
        {
            List<Tuple<Image, Image>> images = new List<Tuple<Image, Image>>();
            FolderBrowserDialog folderBrowserDialog1 = new FolderBrowserDialog();
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                string path = folderBrowserDialog1.SelectedPath;
                string sources = path + "\\Source Images - bmp";
                string gt = path + "\\Ground Truth - bmp";

                string[] source_files = Directory.GetFiles(sources, "*.bmp");
                string[] gt_files = Directory.GetFiles(gt, "*.bmp");
                if (source_files.Length == 0)
                {
                    MessageBox.Show("Aucune image BMP trouvée dans le dossier sélectionné.");
                    return images;
                }

                foreach (string file in source_files)
                {
                    string file_title = Path.GetFileName(file);
                    titres.Add(file_title);
                    Image image = Image.FromFile(file);
                    Image GT = Image.FromFile(gt + "\\" + file_title);
                    Tuple<Image, Image> t = new Tuple<Image, Image>(image, GT);
                    images.Add(t);
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

            Label[] scores = new Label[]
            {
                labelScore1, labelScore2, labelScore3, labelScore4, labelScore5, labelScore6, labelScore7, labelScore8, labelScore9, labelScore10
            };

            while (currentState == State.RUN)
            {
                goToNext(preBoxes, postBoxes, scores);
                Application.DoEvents();
                Thread.Sleep(10);
                // MessageBox.Show("OK");

            }

        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void goToNext(PictureBox[] preBoxes, PictureBox[] postBoxes, Label[] scores)
        {
            Image old = pictureBoxPRE.Image = images[position].Item1;
            labelFichier.Text = titres[position];
            Bitmap sourceBMP = new Bitmap(old);
            Bitmap GTBMP = new Bitmap(images[position].Item2);
            ClImage Img = new ClImage();
            unsafe
            {
                BitmapData sourceBMPData = sourceBMP.LockBits(new Rectangle(0, 0, sourceBMP.Width, sourceBMP.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
                BitmapData GTbmpData = GTBMP.LockBits(new Rectangle(0, 0, GTBMP.Width, GTBMP.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
                Img.objetLibDataImgPtr(5, sourceBMPData.Scan0, GTbmpData.Scan0, sourceBMPData.Stride, sourceBMP.Height, sourceBMP.Width);
                // 1 champ texte retour C++, le seuil auto
                sourceBMP.UnlockBits(sourceBMPData);
            }

            pictureBoxPOST.Image = sourceBMP;
            currentScore = (int)(100 * Math.Sqrt(Img.objetLibValeurChamp(3) * Img.objetLibValeurChamp(4)));
            somme += currentScore;
            moyenne = somme/(position+1);
            labelScore.Text = currentScore  + "%";
            //labelScore.Text = "Score : " + Img.objetLibValeurChamp(0) +";"+ Img.objetLibValeurChamp(1) +";"+ Img.objetLibValeurChamp(2) + "%";
            labelMoyenne.Text = moyenne + "%";
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
            imagesPost.Add(preBoxes[0].Image);

            for (int i = postBoxes.Length - 1; i > 0; i--)
            {
                if (postBoxes[i - 1].Image != null)
                {
                    postBoxes[i].Image = postBoxes[i - 1].Image;
                }

            }
            postBoxes[0].Image = pictureBoxPOST.Image;
            imagesTraitees.Add(postBoxes[0].Image);
            
            for (int i = scores.Length - 1; i > 0; i--)
            {
                if (scores[i - 1].Text != null)
                {
                    scores[i].Text = scores[i - 1].Text;
                }

            }
            scores[0].Text = labelScore.Text;


            position++;
            if (position >= images.Count)
            {
                position = 0;
                processState(State.RUN_STOP);
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

        private void saveImage(List<Image> post, List<Image> pre, string pathPre, string pathPost)
        {
            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();
            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                //string selectedPath = folderBrowserDialog.SelectedPath;
                //string imgTraitees = Path.Combine(selectedPath, "ImgPOST");

                // création dossier pour images PRE et images POST
                if (!Directory.Exists(pathPre))
                {
                    Directory.CreateDirectory(pathPre);
                }
                if (!Directory.Exists(pathPost))
                {
                    Directory.CreateDirectory(pathPost);
                }

                // enregistrement des img
                for (int i = 0; i < pre.Count; i++)
                {
                    string fileName = timeFileName("PRE", i);
                    string filePath = Path.Combine(pathPre, fileName);
                    pre[i].Save(filePath, ImageFormat.Bmp);
                }

                for (int i = 0; i < post.Count; i++)
                {
                    string fileName = timeFileName("POST", i);
                    string filePath = Path.Combine(pathPost, fileName);
                    post[i].Save(filePath, ImageFormat.Bmp);
                }

                MessageBox.Show("Images enregistrées avec succès!");
            }
        }


        private string timeFileName(string nomImg, int index)
        {
            return $"{nomImg}_{index + 1}_{DateTime.Now:yyyy-MM-dd_HH-mm}.bmp";
        }


    }
}
