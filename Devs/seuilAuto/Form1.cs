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
        int moyenne = 0, currentScore = 0, somme = 0;
        
        // Liste des images chargées
        List<String> titres = new List<String>();
        List<Tuple<Image, Image>> loadedFiles = new List<Tuple<Image, Image>>();
        List<ClImage> clImages = new List<ClImage>();

        // Variables pour l'affichage des images
        int positionAffichage = 0; // désactive RUN à la fin de la liste
        PictureBox[] preBoxes = null;
        PictureBox[] postBoxes = null;
        Label[] scores = null;

        // Variables pour le multithreading
        int nbThreads = 2;
        List<Thread> threads = new List<Thread>();
        Queue<ClImage> buffer = new Queue<ClImage>();
        List<int> positionBuffer = new List<int>(); // = -1 arrivé à la fin de la liste
        int bufferLength = 50;

        public enum State
        {
            INIT,
            READY,
            RUN,
            RUN_STOP,
            END_THREADS
        }

        private State currentState = State.INIT;

        // private Thread t1;

        public Form1()
        {
            InitializeComponent();
            processState(State.INIT);
            this.FormClosing += Form1_FormClosing; // Attach the event handler to the FormClosing event
        }

        private void button1_Click(object sender, EventArgs e)
        {
            LoadBmpImages();
            loadFirst();
            processState(State.READY);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            processState(State.RUN);
            affichage();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            processState(State.RUN_STOP);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            // dossier de destination
            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();
            if (folderBrowserDialog.ShowDialog() != DialogResult.OK)
                return;

            // chemin du dossier sélectionné
            string selectedPath = folderBrowserDialog.SelectedPath;

            // chemins des dossiers pour les images PRE et POST
            string preFilePath = Path.Combine(selectedPath, "PreImages");
            string postFilePath = Path.Combine(selectedPath, "PostImages");

            // Créer les dossiers si ils n'existent pas
            if (!Directory.Exists(preFilePath))
                Directory.CreateDirectory(preFilePath);

            if (!Directory.Exists(postFilePath))
                Directory.CreateDirectory(postFilePath);
            saveImage(preFilePath, postFilePath);

            MessageBox.Show("Images sauvegardées avec succès!");
        }


        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            processState(State.END_THREADS);
        }

        //Chargement des images BMP depuis les fichiers
        public void LoadBmpImages()
        {
            FolderBrowserDialog folderBrowserDialog1 = new FolderBrowserDialog();
            if (folderBrowserDialog1.ShowDialog() != DialogResult.OK)
                return;

            string path = folderBrowserDialog1.SelectedPath;
            string sources = path + "\\Source Images - bmp";
            string gt = path + "\\Ground Truth - bmp";

            string[] source_files = Directory.GetFiles(sources, "*.bmp");
            string[] gt_files = Directory.GetFiles(gt, "*.bmp");
            
            if (source_files.Length == 0){
                MessageBox.Show("Aucune image BMP trouvée dans le dossier sélectionné.");
                return;
            }

            foreach (string file in source_files){
                string file_title = Path.GetFileName(file);
                titres.Add(file_title);
                Image image = Image.FromFile(file);
                Image GT = Image.FromFile(gt + "\\" + file_title);

                Tuple<Image, Image> t = new Tuple<Image, Image>(image, GT);
                loadedFiles.Add(t);
            }
        }

        //Chargement de la première image & lancement des threads de traitement
        private void loadFirst(){
            if (loadedFiles.Count == 0)
                return;

            positionAffichage = 0;
            pictureBoxPRE.Image = loadedFiles[positionAffichage].Item1;
            labelNumero.Text = (positionAffichage + 1) + "/" + loadedFiles.Count;
            labelFichier.Text = titres[positionAffichage];

            for (int i = 0; i < nbThreads; i++)
            {
                Thread th = new Thread(thread_suivi_buffer);
                positionBuffer.Add(i);
                th.Start(i);
                threads.Add(th);
            }
            Thread pBar = new Thread(updateBar);
            pBar.Start();
        }

        // Boucle de chaque thread de traitement
        private void thread_suivi_buffer(object id)
        {
            while (currentState != State.END_THREADS)
                if (buffer.Count < bufferLength)
                    avancerBuffer((int)id);
        }

        //Fonction d'appel safe du traitement
        private void avancerBuffer(int id)
        {
            if (positionBuffer[id] == -1)
                return;

            buffer.Enqueue(ClImage.traiter(loadedFiles[positionBuffer[id]]));
            positionBuffer[id] += nbThreads;

            if (positionBuffer[id] >= loadedFiles.Count)
                positionBuffer[id] = -1;

        }

        private void updateBar()
        {
            while (currentState != State.END_THREADS)
            {
                //progressBar1.Value = 100 * buffer.Count / bufferLength;
                Thread.Sleep(50);
            }
        }

        // Lancement du thread d'affichage des images
        private void affichage(){
            preBoxes = new PictureBox[] { pictureBoxPRE1, pictureBoxPRE2, pictureBoxPRE3, pictureBoxPRE4, pictureBoxPRE5, pictureBoxPRE6, pictureBoxPRE7, pictureBoxPRE8, pictureBoxPRE9, pictureBoxPRE10};
            postBoxes = new PictureBox[]{ pictureBoxPOST1, pictureBoxPOST2, pictureBoxPOST3, pictureBoxPOST4, pictureBoxPOST5, pictureBoxPOST6, pictureBoxPOST7, pictureBoxPOST8, pictureBoxPOST9, pictureBoxPOST10};
            scores = new Label[] { labelScore1, labelScore2, labelScore3, labelScore4, labelScore5, labelScore6, labelScore7, labelScore8, labelScore9, labelScore10 };

            while (currentState == State.RUN){
                goToNext();
                Application.DoEvents();
                Thread.Sleep(300);
            }
        }

        // Affichage de l'image suivante
        private void goToNext(){
            if (buffer.Count == 0)
                return;

            ClImage Img = buffer.Dequeue();
           
            Bitmap sourceBMP = new Bitmap(Img.source);
            Bitmap resultBMP = new Bitmap(Img.result);

            // Affichage de l'image source
            pictureBoxPRE.Image = sourceBMP;
            labelFichier.Text = titres[positionAffichage];
            labelNumero.Text = (positionAffichage + 1) + "/" + loadedFiles.Count;



            //Affichage de l'image traitée
            pictureBoxPOST.Image = resultBMP;

            //Gestion du Score
            currentScore = (int)(100 * Math.Max(Img.objetLibValeurChamp(0), Img.objetLibValeurChamp(1)));
            somme += currentScore;
            moyenne = somme / (positionAffichage + 1);

            // Gestion des labels
            labelScore.Text = currentScore + "%";
            labelMoyenne.Text = moyenne + "%";
            labelTemps.Text = $"{Img.tempsTraitement/nbThreads:F2}s";


            // Mise à jour des images précédentes
            for (int i = preBoxes.Length - 1; i > 0; i--)
                if (preBoxes[i - 1].Image != null)
                    preBoxes[i].Image = preBoxes[i - 1].Image;
            
            preBoxes[0].Image = pictureBoxPRE.Image;

            
            for (int i = postBoxes.Length - 1; i > 0; i--)
                if (postBoxes[i - 1].Image != null)
                    postBoxes[i].Image = postBoxes[i - 1].Image;

            postBoxes[0].Image = pictureBoxPOST.Image;

            
            for (int i = scores.Length - 1; i > 0; i--)
                if (scores[i - 1].Text != null)
                    scores[i].Text = scores[i - 1].Text;

            scores[0].Text = labelScore.Text + " (" + labelTemps.Text + ")";


            positionAffichage++;
            if (positionAffichage >= loadedFiles.Count)
            {
                positionAffichage = 0;
                for (int i = 0; i < nbThreads; i++)
                    positionBuffer[i] = i;
                processState(State.RUN_STOP);
                return;
            }
        }

        private void saveImage(string pathPre, string pathPost)
        { 
            if (!Directory.Exists(pathPre) || !Directory.Exists(pathPost))
                return;
            
            // enregistrement des img
            for (int i = 0; i < clImages.Count; i++)
            {
               string fileNamePre = timeFileName("PRE", i);
               string filePathPre = Path.Combine(pathPre, fileNamePre);
               clImages[i].source.Save(filePathPre, ImageFormat.Bmp);

               string fileNamePost = timeFileName("POST", i);
               string filePathPost = Path.Combine(pathPost, fileNamePost);
               clImages[i].result.Save(filePathPost, ImageFormat.Bmp);
            }
            
        }

        private string timeFileName(string nomImg, int index)
        {
            return $"{nomImg}_{index + 1}_{DateTime.Now:yyyy-MM-dd_HH-mm}.bmp";
        }

        private void processState(State newState){
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
