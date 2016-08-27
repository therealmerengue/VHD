#pragma once
#include "VHD.h"
#include <msclr\marshal_cppstd.h>

using namespace msclr::interop;

namespace VHD {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MainForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void) : dyskUtworzony(false), dyskZamontowany(false)
		{
			InitializeComponent();
			this->wczytajDyski();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::TextBox^  textBoxNazwa;
	private: System::Windows::Forms::Button^  buttonUtworzIZamontuj;
	private: System::Windows::Forms::Button^  buttonOpenFileDialog;
	private: System::Windows::Forms::TextBox^  textBoxFolder;
	private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialog;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::GroupBox^  groupBoxUtworzDysk;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  textBoxRozmiar;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  buttonOdswiezDyski;
	private: System::Windows::Forms::GroupBox^  groupBoxZamontujDysk;
	private: System::Windows::Forms::Button^  buttonWybierzDysk;
	private: System::Windows::Forms::Button^  buttonZamontujUtworzony;
	private: System::Windows::Forms::TextBox^  textBoxNazwaDysku;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog;
	private: System::Windows::Forms::Button^  buttonPoliczDyski;
	private: System::Windows::Forms::GroupBox^  groupBoxWybierzDysk;
	private: System::Windows::Forms::Button^  buttonWybierzDyskZListy;
	private: System::Windows::Forms::TextBox^  textBoxWybranyDysk;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::ListBox^  listBoxDyski;

	private:
		String^ wybranyDysk;
		String^ sciezkaDoDysku;
		bool dyskUtworzony, dyskZamontowany;
	private: System::Windows::Forms::GroupBox^  groupBoxSortowanie;

	private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialogFoldery;
	private: System::Windows::Forms::Button^  buttonSortuj;

	private: System::Windows::Forms::TextBox^  textBoxFolderDoPosortowania;

	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Button^  buttonDodajFoldery;

		void wczytajDyski()
		{
			this->listBoxDyski->Items->Clear();
			for (int i = 0; i < System::IO::DriveInfo::GetDrives()->Length; i++)
			{
				Object^ dysk = System::IO::DriveInfo::GetDrives()->GetValue(i);
				this->listBoxDyski->Items->Add(dysk);
			}
		}
		bool sprawdzSciezke(String^ sciezka)
		{
			std::string rozszerzenie = ".vhd";
			std::string strSciezka = marshal_as<std::string>(sciezka);
			return (0 == strSciezka.compare(strSciezka.length() - rozszerzenie.length(), rozszerzenie.length(), rozszerzenie));
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->textBoxNazwa = (gcnew System::Windows::Forms::TextBox());
			this->buttonUtworzIZamontuj = (gcnew System::Windows::Forms::Button());
			this->buttonOpenFileDialog = (gcnew System::Windows::Forms::Button());
			this->textBoxFolder = (gcnew System::Windows::Forms::TextBox());
			this->folderBrowserDialog = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBoxUtworzDysk = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxRozmiar = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->listBoxDyski = (gcnew System::Windows::Forms::ListBox());
			this->buttonOdswiezDyski = (gcnew System::Windows::Forms::Button());
			this->groupBoxZamontujDysk = (gcnew System::Windows::Forms::GroupBox());
			this->buttonWybierzDysk = (gcnew System::Windows::Forms::Button());
			this->buttonZamontujUtworzony = (gcnew System::Windows::Forms::Button());
			this->textBoxNazwaDysku = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->openFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->buttonPoliczDyski = (gcnew System::Windows::Forms::Button());
			this->groupBoxWybierzDysk = (gcnew System::Windows::Forms::GroupBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->textBoxWybranyDysk = (gcnew System::Windows::Forms::TextBox());
			this->buttonWybierzDyskZListy = (gcnew System::Windows::Forms::Button());
			this->groupBoxSortowanie = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxFolderDoPosortowania = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->buttonSortuj = (gcnew System::Windows::Forms::Button());
			this->buttonDodajFoldery = (gcnew System::Windows::Forms::Button());
			this->folderBrowserDialogFoldery = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->groupBoxUtworzDysk->SuspendLayout();
			this->groupBoxZamontujDysk->SuspendLayout();
			this->groupBoxWybierzDysk->SuspendLayout();
			this->groupBoxSortowanie->SuspendLayout();
			this->SuspendLayout();
			// 
			// textBoxNazwa
			// 
			this->textBoxNazwa->Location = System::Drawing::Point(120, 56);
			this->textBoxNazwa->Name = L"textBoxNazwa";
			this->textBoxNazwa->Size = System::Drawing::Size(172, 22);
			this->textBoxNazwa->TabIndex = 1;
			// 
			// buttonUtworzIZamontuj
			// 
			this->buttonUtworzIZamontuj->Location = System::Drawing::Point(143, 122);
			this->buttonUtworzIZamontuj->Name = L"buttonUtworzIZamontuj";
			this->buttonUtworzIZamontuj->Size = System::Drawing::Size(149, 30);
			this->buttonUtworzIZamontuj->TabIndex = 2;
			this->buttonUtworzIZamontuj->Text = L"Utwórz i zamontuj";
			this->buttonUtworzIZamontuj->UseVisualStyleBackColor = true;
			this->buttonUtworzIZamontuj->Click += gcnew System::EventHandler(this, &MainForm::buttonUtworzVHD_Click);
			// 
			// buttonOpenFileDialog
			// 
			this->buttonOpenFileDialog->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->buttonOpenFileDialog->Location = System::Drawing::Point(298, 25);
			this->buttonOpenFileDialog->Name = L"buttonOpenFileDialog";
			this->buttonOpenFileDialog->Size = System::Drawing::Size(144, 30);
			this->buttonOpenFileDialog->TabIndex = 4;
			this->buttonOpenFileDialog->Text = L"Wybierz";
			this->buttonOpenFileDialog->UseVisualStyleBackColor = true;
			this->buttonOpenFileDialog->Click += gcnew System::EventHandler(this, &MainForm::buttonOpenFileDialog_Click);
			// 
			// textBoxFolder
			// 
			this->textBoxFolder->Location = System::Drawing::Point(120, 29);
			this->textBoxFolder->Name = L"textBoxFolder";
			this->textBoxFolder->ReadOnly = true;
			this->textBoxFolder->Size = System::Drawing::Size(172, 22);
			this->textBoxFolder->TabIndex = 5;
			// 
			// folderBrowserDialog
			// 
			this->folderBrowserDialog->RootFolder = System::Environment::SpecialFolder::MyComputer;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 32);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(108, 17);
			this->label1->TabIndex = 6;
			this->label1->Text = L"Wybrany folder:";
			// 
			// groupBoxUtworzDysk
			// 
			this->groupBoxUtworzDysk->Controls->Add(this->textBoxRozmiar);
			this->groupBoxUtworzDysk->Controls->Add(this->label3);
			this->groupBoxUtworzDysk->Controls->Add(this->label2);
			this->groupBoxUtworzDysk->Controls->Add(this->buttonUtworzIZamontuj);
			this->groupBoxUtworzDysk->Controls->Add(this->textBoxFolder);
			this->groupBoxUtworzDysk->Controls->Add(this->textBoxNazwa);
			this->groupBoxUtworzDysk->Controls->Add(this->label1);
			this->groupBoxUtworzDysk->Controls->Add(this->buttonOpenFileDialog);
			this->groupBoxUtworzDysk->Location = System::Drawing::Point(11, 12);
			this->groupBoxUtworzDysk->Name = L"groupBoxUtworzDysk";
			this->groupBoxUtworzDysk->Size = System::Drawing::Size(448, 173);
			this->groupBoxUtworzDysk->TabIndex = 7;
			this->groupBoxUtworzDysk->TabStop = false;
			this->groupBoxUtworzDysk->Text = L"Utwórz dysk";
			// 
			// textBoxRozmiar
			// 
			this->textBoxRozmiar->Location = System::Drawing::Point(120, 82);
			this->textBoxRozmiar->Name = L"textBoxRozmiar";
			this->textBoxRozmiar->Size = System::Drawing::Size(172, 22);
			this->textBoxRozmiar->TabIndex = 9;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(7, 85);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(98, 17);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Rozmiar (MB):";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(7, 59);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(54, 17);
			this->label2->TabIndex = 7;
			this->label2->Text = L"Nazwa:";
			// 
			// listBoxDyski
			// 
			this->listBoxDyski->FormattingEnabled = true;
			this->listBoxDyski->ItemHeight = 16;
			this->listBoxDyski->Location = System::Drawing::Point(10, 23);
			this->listBoxDyski->Name = L"listBoxDyski";
			this->listBoxDyski->Size = System::Drawing::Size(242, 148);
			this->listBoxDyski->TabIndex = 9;
			// 
			// buttonOdswiezDyski
			// 
			this->buttonOdswiezDyski->Location = System::Drawing::Point(267, 23);
			this->buttonOdswiezDyski->Name = L"buttonOdswiezDyski";
			this->buttonOdswiezDyski->Size = System::Drawing::Size(175, 30);
			this->buttonOdswiezDyski->TabIndex = 10;
			this->buttonOdswiezDyski->Text = L"Odśwież";
			this->buttonOdswiezDyski->UseVisualStyleBackColor = true;
			this->buttonOdswiezDyski->Click += gcnew System::EventHandler(this, &MainForm::buttonOdswiezDyski_Click);
			// 
			// groupBoxZamontujDysk
			// 
			this->groupBoxZamontujDysk->Controls->Add(this->buttonWybierzDysk);
			this->groupBoxZamontujDysk->Controls->Add(this->buttonZamontujUtworzony);
			this->groupBoxZamontujDysk->Controls->Add(this->textBoxNazwaDysku);
			this->groupBoxZamontujDysk->Controls->Add(this->label4);
			this->groupBoxZamontujDysk->Location = System::Drawing::Point(465, 12);
			this->groupBoxZamontujDysk->Name = L"groupBoxZamontujDysk";
			this->groupBoxZamontujDysk->Size = System::Drawing::Size(444, 173);
			this->groupBoxZamontujDysk->TabIndex = 13;
			this->groupBoxZamontujDysk->TabStop = false;
			this->groupBoxZamontujDysk->Text = L"Zamontuj dysk";
			// 
			// buttonWybierzDysk
			// 
			this->buttonWybierzDysk->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->buttonWybierzDysk->Location = System::Drawing::Point(327, 25);
			this->buttonWybierzDysk->Name = L"buttonWybierzDysk";
			this->buttonWybierzDysk->Size = System::Drawing::Size(111, 30);
			this->buttonWybierzDysk->TabIndex = 10;
			this->buttonWybierzDysk->Text = L"Wybierz";
			this->buttonWybierzDysk->UseVisualStyleBackColor = true;
			this->buttonWybierzDysk->Click += gcnew System::EventHandler(this, &MainForm::buttonWybierzDysk_Click);
			// 
			// buttonZamontujUtworzony
			// 
			this->buttonZamontujUtworzony->Location = System::Drawing::Point(174, 122);
			this->buttonZamontujUtworzony->Name = L"buttonZamontujUtworzony";
			this->buttonZamontujUtworzony->Size = System::Drawing::Size(93, 30);
			this->buttonZamontujUtworzony->TabIndex = 14;
			this->buttonZamontujUtworzony->Text = L"Zamontuj";
			this->buttonZamontujUtworzony->UseVisualStyleBackColor = true;
			this->buttonZamontujUtworzony->Click += gcnew System::EventHandler(this, &MainForm::buttonZamontujUtworzony_Click);
			// 
			// textBoxNazwaDysku
			// 
			this->textBoxNazwaDysku->Location = System::Drawing::Point(116, 29);
			this->textBoxNazwaDysku->Name = L"textBoxNazwaDysku";
			this->textBoxNazwaDysku->ReadOnly = true;
			this->textBoxNazwaDysku->Size = System::Drawing::Size(205, 22);
			this->textBoxNazwaDysku->TabIndex = 10;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(2, 32);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(95, 17);
			this->label4->TabIndex = 11;
			this->label4->Text = L"Nazwa dysku:";
			// 
			// openFileDialog
			// 
			this->openFileDialog->FileName = L"openFileDialog1";
			// 
			// buttonPoliczDyski
			// 
			this->buttonPoliczDyski->Location = System::Drawing::Point(267, 134);
			this->buttonPoliczDyski->Name = L"buttonPoliczDyski";
			this->buttonPoliczDyski->Size = System::Drawing::Size(175, 32);
			this->buttonPoliczDyski->TabIndex = 14;
			this->buttonPoliczDyski->Text = L"Liczba dysków";
			this->buttonPoliczDyski->UseVisualStyleBackColor = true;
			this->buttonPoliczDyski->Click += gcnew System::EventHandler(this, &MainForm::buttonPoliczDyski_Click);
			// 
			// groupBoxWybierzDysk
			// 
			this->groupBoxWybierzDysk->Controls->Add(this->label5);
			this->groupBoxWybierzDysk->Controls->Add(this->buttonPoliczDyski);
			this->groupBoxWybierzDysk->Controls->Add(this->textBoxWybranyDysk);
			this->groupBoxWybierzDysk->Controls->Add(this->buttonWybierzDyskZListy);
			this->groupBoxWybierzDysk->Controls->Add(this->listBoxDyski);
			this->groupBoxWybierzDysk->Controls->Add(this->buttonOdswiezDyski);
			this->groupBoxWybierzDysk->Location = System::Drawing::Point(11, 191);
			this->groupBoxWybierzDysk->Name = L"groupBoxWybierzDysk";
			this->groupBoxWybierzDysk->Size = System::Drawing::Size(448, 177);
			this->groupBoxWybierzDysk->TabIndex = 15;
			this->groupBoxWybierzDysk->TabStop = false;
			this->groupBoxWybierzDysk->Text = L"Wybierz dysk";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(264, 102);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(101, 17);
			this->label5->TabIndex = 10;
			this->label5->Text = L"Wybrany dysk:";
			// 
			// textBoxWybranyDysk
			// 
			this->textBoxWybranyDysk->Location = System::Drawing::Point(371, 99);
			this->textBoxWybranyDysk->Name = L"textBoxWybranyDysk";
			this->textBoxWybranyDysk->ReadOnly = true;
			this->textBoxWybranyDysk->Size = System::Drawing::Size(71, 22);
			this->textBoxWybranyDysk->TabIndex = 12;
			// 
			// buttonWybierzDyskZListy
			// 
			this->buttonWybierzDyskZListy->Location = System::Drawing::Point(267, 59);
			this->buttonWybierzDyskZListy->Name = L"buttonWybierzDyskZListy";
			this->buttonWybierzDyskZListy->Size = System::Drawing::Size(175, 30);
			this->buttonWybierzDyskZListy->TabIndex = 11;
			this->buttonWybierzDyskZListy->Text = L"Wybierz";
			this->buttonWybierzDyskZListy->UseVisualStyleBackColor = true;
			this->buttonWybierzDyskZListy->Click += gcnew System::EventHandler(this, &MainForm::buttonWybierzDyskZListy_Click);
			// 
			// groupBoxSortowanie
			// 
			this->groupBoxSortowanie->Controls->Add(this->textBoxFolderDoPosortowania);
			this->groupBoxSortowanie->Controls->Add(this->label6);
			this->groupBoxSortowanie->Controls->Add(this->buttonSortuj);
			this->groupBoxSortowanie->Controls->Add(this->buttonDodajFoldery);
			this->groupBoxSortowanie->Location = System::Drawing::Point(465, 191);
			this->groupBoxSortowanie->Name = L"groupBoxSortowanie";
			this->groupBoxSortowanie->Size = System::Drawing::Size(443, 176);
			this->groupBoxSortowanie->TabIndex = 16;
			this->groupBoxSortowanie->TabStop = false;
			this->groupBoxSortowanie->Text = L"Sortuj";
			// 
			// textBoxFolderDoPosortowania
			// 
			this->textBoxFolderDoPosortowania->Location = System::Drawing::Point(172, 26);
			this->textBoxFolderDoPosortowania->Name = L"textBoxFolderDoPosortowania";
			this->textBoxFolderDoPosortowania->ReadOnly = true;
			this->textBoxFolderDoPosortowania->Size = System::Drawing::Size(148, 22);
			this->textBoxFolderDoPosortowania->TabIndex = 10;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(6, 29);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(160, 17);
			this->label6->TabIndex = 10;
			this->label6->Text = L"Folder do posortowania:";
			// 
			// buttonSortuj
			// 
			this->buttonSortuj->Location = System::Drawing::Point(173, 133);
			this->buttonSortuj->Name = L"buttonSortuj";
			this->buttonSortuj->Size = System::Drawing::Size(91, 31);
			this->buttonSortuj->TabIndex = 1;
			this->buttonSortuj->Text = L"Sortuj";
			this->buttonSortuj->UseVisualStyleBackColor = true;
			this->buttonSortuj->Click += gcnew System::EventHandler(this, &MainForm::buttonSortuj_Click);
			// 
			// buttonDodajFoldery
			// 
			this->buttonDodajFoldery->Location = System::Drawing::Point(326, 22);
			this->buttonDodajFoldery->Name = L"buttonDodajFoldery";
			this->buttonDodajFoldery->Size = System::Drawing::Size(111, 30);
			this->buttonDodajFoldery->TabIndex = 0;
			this->buttonDodajFoldery->Text = L"Dodaj foldery";
			this->buttonDodajFoldery->UseVisualStyleBackColor = true;
			this->buttonDodajFoldery->Click += gcnew System::EventHandler(this, &MainForm::buttonDodajFoldery_Click);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(916, 503);
			this->Controls->Add(this->groupBoxSortowanie);
			this->Controls->Add(this->groupBoxWybierzDysk);
			this->Controls->Add(this->groupBoxZamontujDysk);
			this->Controls->Add(this->groupBoxUtworzDysk);
			this->MaximizeBox = false;
			this->Name = L"MainForm";
			this->Text = L"VHD";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &MainForm::MainForm_FormClosed);
			this->groupBoxUtworzDysk->ResumeLayout(false);
			this->groupBoxUtworzDysk->PerformLayout();
			this->groupBoxZamontujDysk->ResumeLayout(false);
			this->groupBoxZamontujDysk->PerformLayout();
			this->groupBoxWybierzDysk->ResumeLayout(false);
			this->groupBoxWybierzDysk->PerformLayout();
			this->groupBoxSortowanie->ResumeLayout(false);
			this->groupBoxSortowanie->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void buttonUtworzVHD_Click(System::Object^  sender, System::EventArgs^  e) {

		if (!this->dyskUtworzony && !this->dyskZamontowany)
		{
			if (!System::String::IsNullOrWhiteSpace(this->textBoxNazwa->Text) && !System::String::IsNullOrWhiteSpace(this->textBoxFolder->Text) && !System::String::IsNullOrWhiteSpace(this->textBoxRozmiar->Text))
			{
				std::string strFolder = marshal_as<std::string>(this->textBoxFolder->Text);
				std::string strNazwa = marshal_as<std::string>(this->textBoxNazwa->Text);
				if (strFolder.back() != '\\')
					strFolder.append("\\");

				std::wstring wstrNazwa = s2ws(strFolder + strNazwa);
				PCWSTR nazwaDysku = wstrNazwa.c_str();
				int rozmiar;
				if (int::TryParse(this->textBoxRozmiar->Text, rozmiar))
				{
					if (CreateVHD_Fixed(nazwaDysku, rozmiar))
					{
						BOOL suc = OpenAndAttachVHD2(nazwaDysku, countPhysicalDisks()); //wtf?
						this->sciezkaDoDysku = marshal_as<String^>(strFolder + strNazwa);
						dyskUtworzony = true;
						dyskZamontowany = true;
					}
					else
						MessageBox::Show("Blad przy tworzeniu dysku.");
				}
				else
				{
					MessageBox::Show("Bledny rozmiar dysku.");
					this->textBoxRozmiar->Clear();
				}
			}
			else
				MessageBox::Show("Niepoprawne dane.");
		}
		else
			MessageBox::Show("Dysk zostal juz utworzony.");
		
		this->textBoxFolder->Clear();
		this->textBoxNazwa->Clear();
		this->textBoxRozmiar->Clear();
	}

	private: System::Void buttonOpenFileDialog_Click(System::Object^  sender, System::EventArgs^  e) {
		this->folderBrowserDialog->ShowDialog();
		this->textBoxFolder->Text = this->folderBrowserDialog->SelectedPath;
	}

	private: System::Void buttonOdswiezDyski_Click(System::Object^  sender, System::EventArgs^  e) {
		this->wczytajDyski();
	}

	private: System::Void buttonWybierzDysk_Click(System::Object^  sender, System::EventArgs^  e) {
		this->openFileDialog->ShowDialog();
		String^ nazwaDysku = this->openFileDialog->InitialDirectory + this->openFileDialog->FileName;
		if (this->sprawdzSciezke(nazwaDysku))
			this->textBoxNazwaDysku->Text = this->openFileDialog->InitialDirectory + this->openFileDialog->FileName;
		else
			MessageBox::Show("Wybierz wlasciwy plik.");
	}

	private: System::Void buttonZamontujUtworzony_Click(System::Object^  sender, System::EventArgs^  e) {

		if (!this->dyskZamontowany)
		{
			if (!System::String::IsNullOrWhiteSpace(this->textBoxNazwaDysku->Text))
			{
				marshal_context^ context = gcnew marshal_context();
				std::string nazwaDysku = context->marshal_as<std::string>(this->textBoxNazwaDysku->Text);
				std::wstring wstrNazwaDysku = s2ws(nazwaDysku);
				PCWSTR pcwstrNazwaDysku = wstrNazwaDysku.c_str();
				this->sciezkaDoDysku = this->textBoxNazwaDysku->Text;
				OpenAndAttachVHD2(pcwstrNazwaDysku, countPhysicalDisks());
				this->dyskZamontowany = true;
			}
		}
		else
			MessageBox::Show("Dysk zostal juz zamontowany.");
	}

	private: System::Void buttonPoliczDyski_Click(System::Object^  sender, System::EventArgs^  e) {
		MessageBox::Show(countPhysicalDisks().ToString());
	}

	private: System::Void buttonWybierzDyskZListy_Click(System::Object^  sender, System::EventArgs^  e) {
		if (this->listBoxDyski->SelectedItems->Count != 0)
		{
			this->wybranyDysk = this->listBoxDyski->SelectedItems[0]->ToString();
			this->textBoxWybranyDysk->Text = this->wybranyDysk;
			this->listBoxDyski->ClearSelected();
			this->folderBrowserDialogFoldery->SelectedPath = this->wybranyDysk;
		}
	}

	private: System::Void MainForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
		/*if (this->sciezkaDoDysku != nullptr)
		{
			marshal_context^ context = gcnew marshal_context();
			std::string sciezka = context->marshal_as<std::string>(this->sciezkaDoDysku);
			OpenAndDetachVHD(s2ws(sciezka).c_str());
		}*/
	}

	private: System::Void buttonDodajFoldery_Click(System::Object^  sender, System::EventArgs^  e) {
		this->folderBrowserDialogFoldery->ShowDialog();
		this->textBoxFolderDoPosortowania->Text = this->folderBrowserDialogFoldery->SelectedPath;
	}

	private: System::Void buttonSortuj_Click(System::Object^  sender, System::EventArgs^  e) {
		marshal_context^ context = gcnew marshal_context();
		std::string dysk = context->marshal_as<std::string>(this->wybranyDysk);
		std::string folderDoPosortowania = context->marshal_as<std::string>(this->textBoxFolderDoPosortowania->Text);
		std::vector<std::string> pliki;
		getFilesInDirectory(folderDoPosortowania.c_str(), pliki);
		sortuj(pliki, dysk, folderDoPosortowania);
	}
};
}

