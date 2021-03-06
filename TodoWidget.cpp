#include "ToDoWidget.h"

void
ToDoWidget::CreatingWidgetsFromData()
{
    //NOTE(hugo): This assumes that m_categories has
    // the right datas in it
    for(int CategoryIndex = 0;
	CategoryIndex < m_categories.size();
	++CategoryIndex)
    {
	ToDoCategory* CurrentCategory = m_categories[CategoryIndex];
	ToDoCategoryWidget* CurrentCategoryWidget = new ToDoCategoryWidget();

	CurrentCategoryWidget->GroupBox =
	    new QGroupBox(CurrentCategory->name, this);
	CurrentCategoryWidget->GroupBox->
	    setStyleSheet("color: " + color(CategoryIndex));
	CurrentCategoryWidget->Layout = new QVBoxLayout(this);
	CurrentCategoryWidget->Items =  new QListWidget(this);
	
	for(int ItemIndex = 0;
	    ItemIndex < CurrentCategory->items.size();
	    ++ItemIndex)
	{
	    CurrentCategoryWidget->Items->addItem(new QListWidgetItem(CurrentCategory->items[ItemIndex],
								      CurrentCategoryWidget->Items));
	}
        
	CurrentCategoryWidget->Layout->
	    addWidget(CurrentCategoryWidget->Items);
	CurrentCategoryWidget->GroupBox->
	    setLayout(CurrentCategoryWidget->Layout);
	m_mainGroupBoxLayout->
	    addWidget(CurrentCategoryWidget->GroupBox);
	
	m_QCategories.append(CurrentCategoryWidget);
    }    
}

ToDoWidget::ToDoWidget(QString filename, QWidget* window)
    : m_filename(filename), m_categories(), m_QCategories()
{
    QWidget();

    if(!m_filename.isEmpty())
	LoadFile();
    
    //NOTE(hugo): Creating the main groupBox containing all the 
    //IMPORTANT(hugo): What should the parent of this widget be ?
    m_mainGroupBox = new QGroupBox("ToDo", window);

    m_mainGroupBoxLayout = new QHBoxLayout(this);

    CreatingWidgetsFromData();

    m_mainGroupBox->setLayout(m_mainGroupBoxLayout);
}

ToDoWidget::~ToDoWidget()
{

}


void
ToDoWidget::LoadFile()
{
    QFile file(m_filename);
    if(file.open(QIODevice::ReadOnly))
    {
	QTextStream in(&file);
	uint CategoryCount = in.readLine().toUInt();

	for(int CategoryIndex = 0;
	    CategoryIndex < CategoryCount;
	    ++CategoryIndex)
	{
	    //NOTE(hugo): Processing the current category in the file
	    ToDoCategory *category = new ToDoCategory();
	    QString categoryLine = in.readLine();
	    QStringList CategoryLineSplitted = categoryLine.split(" ");
	    uint itemCount = CategoryLineSplitted[0].toUInt();
	    category->name = CategoryLineSplitted[1];
	    for(int ItemIndex = 0;
		ItemIndex < itemCount;
		++ItemIndex)
	    {
		category->items.append(in.readLine());
	    }

	    m_categories.append(category);
	}
	file.close();
    }
    else
    {
	qDebug() << "Could not open file";
    }
}

void
ToDoWidget::addToDoItem()
{
    QString CategoryName = categoryChoice->currentText();
    QString ItemName = toDoNameLineEdit->text();
    
    for(int CategoryIndex = 0;
	CategoryIndex < m_categories.size();
	CategoryIndex++)
    {
	
	if(m_categories[CategoryIndex]->name == CategoryName)
	{
	    m_categories[CategoryIndex]->items.push_back(ItemName);
	    
	    //NOTE(hugo): I assume that m_categories and m_QCategories
	    // are correlated concerning the category they represent
	    
	    m_QCategories[CategoryIndex]->Items->addItem(
		       new QListWidgetItem(ItemName,
					   m_QCategories[CategoryIndex]->Items));
	    break;
	}
    }

    delete actionWindow;

    if(!m_filename.isEmpty())
	SaveFile();
}

void
ToDoWidget::deleteToDoItem()
{
    QString ItemName = todoChoice->currentText();
    bool found = false;
    for(int CategoryIndex = 0;
	CategoryIndex < m_categories.size();
	CategoryIndex++)
    {
	int FoundIndex = -1;
	for(int ItemIndex = 0;
	    ItemIndex < m_categories[CategoryIndex]->
		items.size();
	    ItemIndex++)
	{
	    QString currentItem = m_categories[CategoryIndex]->
		items[ItemIndex];
	    if(QString::compare(currentItem,
				ItemName)
	       == 0)
	    {
		FoundIndex = ItemIndex;
		m_categories[CategoryIndex]->
		    items.remove(ItemIndex);
		found = true;
		break;
	    }
	}

	if(FoundIndex != -1)
	    m_QCategories[CategoryIndex]->
		Items->takeItem(FoundIndex);
	
    }
    if(!found)
	QMessageBox::warning(0,
			     "ToDo not found",
			     "ToDo not found");

    delete actionWindow;
    if(!m_filename.isEmpty())
	SaveFile();
}


void
ToDoWidget::SaveFile()
{
    //NOTE(hugo) : To avoid any issues, we delete the file
    // each time so that it is not overwritten
    QFile::remove(m_filename);
    
    QString textFile;

    //NOTE(hugo): computing textFile
    textFile += QString::number(m_categories.size());
    textFile += "\n";
    for(int CatIndex = 0;
	CatIndex < m_categories.size();
	CatIndex++)
    {
	ToDoCategory* currentCategory = m_categories[CatIndex];
	textFile += QString::number(currentCategory->
				    items.size());
	textFile += " ";
	textFile += currentCategory->name;
	textFile += "\n";
	
	for(int ItemIndex = 0;
	    ItemIndex < currentCategory->items.size();
	    ItemIndex++)
	{
	    textFile += currentCategory->items[ItemIndex];
	    textFile += "\n";
	}
    }
    
    QFile file(m_filename);
    if(file.open(QIODevice::ReadWrite))
    {
	QTextStream stream(&file);
	stream << textFile;
    }
    else
    {
	qDebug("Failing to open the file.");
    }
    
}

void
ToDoWidget::addPressed()
{
    
    actionWindow = new QWidget();
    actionWindow->setVisible(true);
    actionWindow->setWindowTitle("What to add ?");
    
    actionWindow->resize(200,100);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(actionWindow);
    QLabel* catNameLabel = new QLabel("Category Name",
				      actionWindow);
    //catNameLineEdit = new QLineEdit(actionWindow);
    categoryChoice = new QComboBox(actionWindow);
    for(int CategoryIndex = 0;
	CategoryIndex < m_categories.size();
	++CategoryIndex)
    {
	categoryChoice->addItem(m_categories[CategoryIndex]->name);
    }
    QLabel* toDoNameLabel = new QLabel("ToDo Name",
				       actionWindow);
    toDoNameLineEdit = new QLineEdit(actionWindow);
    
    QPushButton* okButton = new QPushButton("Valider",
					    actionWindow);
    
    
    mainLayout->addWidget(catNameLabel);
    mainLayout->addWidget(categoryChoice);
    mainLayout->addWidget(toDoNameLabel);
    mainLayout->addWidget(toDoNameLineEdit);
    mainLayout->addWidget(okButton);
    
    actionWindow->setLayout(mainLayout);

    QObject::connect(okButton,
		     SIGNAL(clicked()),
		     this,
		     SLOT(addToDoItem()));
}


void
ToDoWidget::delPressed()
{
    
    actionWindow = new QWidget();
    actionWindow->setVisible(true);
    actionWindow->setWindowTitle("What to delete ?");

    actionWindow->resize(200,100);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(actionWindow);
    QLabel* toDoNameLabel = new QLabel("ToDo Name",
				       actionWindow);
    //toDoNameLineEdit = new QLineEdit(actionWindow);
    todoChoice = new QComboBox(actionWindow);
    for(int CategoryIndex = 0;
	CategoryIndex < m_categories.size();
	++CategoryIndex)
    {
	for(int TodoIndex = 0;
	    TodoIndex < m_categories[CategoryIndex]->items.size();
	    ++TodoIndex)
	{
	    todoChoice->addItem(m_categories[CategoryIndex]
				->items[TodoIndex]);
	}
    }
    
    QPushButton* okButton = new QPushButton("Valider",
					    actionWindow);
    
    mainLayout->addWidget(toDoNameLabel);
    mainLayout->addWidget(todoChoice);
    mainLayout->addWidget(okButton);
    
    actionWindow->setLayout(mainLayout);
    
    QObject::connect(okButton,
		     SIGNAL(clicked()),
		     this,
		     SLOT(deleteToDoItem()));
}

void
ToDoWidget::addCatPressed()
{
    actionWindow = new QWidget();
    actionWindow->setVisible(true);
    actionWindow->setWindowTitle("Category Name");

    actionWindow->resize(200,100);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(actionWindow);
    QLabel* catNameLabel = new QLabel("Category Name",
				       actionWindow);
    catNameLineEdit = new QLineEdit(actionWindow);
    
    QPushButton* okButton = new QPushButton("Valider",
					    actionWindow);
    
    mainLayout->addWidget(catNameLabel);
    mainLayout->addWidget(catNameLineEdit);
    mainLayout->addWidget(okButton);
    
    actionWindow->setLayout(mainLayout);
    
    QObject::connect(okButton,
		     SIGNAL(clicked()),
		     this,
		     SLOT(addCategory()));
}

void
ToDoWidget::delCatPressed()
{
    actionWindow = new QWidget();
    actionWindow->setVisible(true);
    actionWindow->setWindowTitle("What to delete ?");

    actionWindow->resize(200,100);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(actionWindow);
    QLabel* catNameLabel = new QLabel("Category Name",
				       actionWindow);
    //catNameLineEdit = new QLineEdit(actionWindow);
    categoryChoice = new QComboBox(actionWindow);
    for(int CategoryIndex = 0;
	CategoryIndex < m_categories.size();
	++CategoryIndex)
    {
	categoryChoice->addItem(m_categories[CategoryIndex]->name);
    }    
    
    QPushButton* okButton = new QPushButton("Valider",
					    actionWindow);
    
    mainLayout->addWidget(catNameLabel);
    mainLayout->addWidget(categoryChoice);
    mainLayout->addWidget(okButton);
    
    actionWindow->setLayout(mainLayout);
    
    QObject::connect(okButton,
		     SIGNAL(clicked()),
		     this,
		     SLOT(deleteCategory()));
}

void
ToDoWidget::addCategory()
{
    QString catName = catNameLineEdit->text();
    ToDoCategory* newCategory = new ToDoCategory();
    newCategory->name = catName;
    newCategory->items = QVector<QString>();

    m_categories.append(newCategory);

    ToDoCategoryWidget* newQCategory = new ToDoCategoryWidget();
    newQCategory->GroupBox = new QGroupBox(catName,
					   this);
    newQCategory->GroupBox->
	setStyleSheet("color: " + color(m_categories.size()-1));
    newQCategory->Layout = new QVBoxLayout(this);
    newQCategory->Items = new QListWidget(this);

    newQCategory->Layout->addWidget(newQCategory->Items);
    newQCategory->GroupBox->setLayout(newQCategory->Layout);
    m_mainGroupBoxLayout->addWidget(newQCategory->GroupBox);

    m_QCategories.append(newQCategory);

    
    delete actionWindow;

    if(!m_filename.isEmpty())
	SaveFile();
}

void
ToDoWidget::deleteCategory()
{

    QString categoryName = categoryChoice->currentText();
    int FoundIndex = -1;

    for(int CatIndex = 0;
	CatIndex < m_categories.size();
	CatIndex++)
    {
	QString currentCategoryName =
	    m_categories[CatIndex]->name;
	if(QString::compare(currentCategoryName,
			    categoryName) == 0)
	{
	    FoundIndex = CatIndex;
	    break;
	}
    }

    if(FoundIndex != -1)
    {
	m_mainGroupBoxLayout->removeWidget(
		 m_QCategories[FoundIndex]->GroupBox);
	
	delete m_QCategories[FoundIndex]->GroupBox;
	
	m_categories[FoundIndex]->items.clear();
	
	delete m_categories[FoundIndex];
	delete m_QCategories[FoundIndex];
	m_categories.remove(FoundIndex);
	m_QCategories.remove(FoundIndex);
    }
    else
    {
	QMessageBox::warning(0,
			     "Category not found",
			     "Category not found");
    }
    
    delete actionWindow;

    if(!m_filename.isEmpty())
	SaveFile();
}

QString
ToDoWidget::color(int i)
{
    if(i == 0)
	return "blue";
    if(i == 1)
	return "darkorange";
    if(i == 2)
	return "green";
    if(i == 3)
	return "gold";
    if(i == 4)
	return "red";

    return "black";
}

void
ToDoWidget::openFilePressed()
{
    QFileDialog* fileDialog = new QFileDialog(0,Qt::Window);

    QStringList filters;
    filters << "Text file (*.txt)";
    
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog->setNameFilters(filters);
    fileDialog->setVisible(true);

    QObject::connect(fileDialog, SIGNAL(currentChanged(QString)),
		     this, SLOT(changeCache(QString)));
    QObject::connect(fileDialog, SIGNAL(accepted()),
		     this, SLOT(openFile()));
}

void ToDoWidget::saveAsPressed()
{
    QString saveName = QFileDialog::
	getSaveFileName(0,
			QString("Save File"),
			QString(),
			QString("Text file (*.txt)"));
    if(!saveName.isEmpty())
    {
	m_filename = saveName;
	SaveFile();
    }
    
}

void
ToDoWidget::changeCache(QString newString)
{
    if(!cacheString)
	delete cacheString;
    cacheString = new QString(newString);
    qDebug(newString.toLatin1());
}

void
clearLayout(QLayout* layout)
{
    if(layout)
    {
	QLayoutItem* item;
	while((item = layout->takeAt(0)) != NULL)
	{
	    delete item->widget();
	    delete item;
	}
    }

}

void
ToDoWidget::openFile()
{
    clearLayout(m_mainGroupBoxLayout);

    m_categories.clear();
    m_QCategories.clear();

    m_filename = *cacheString;
    LoadFile();

    CreatingWidgetsFromData();
    
}

void
ToDoWidget::newFilePressed()
{
    clearLayout(m_mainGroupBoxLayout);
    m_categories.clear();
    m_QCategories.clear();
    m_filename = QString();
    
}


#include "ToDoWidget.moc"
		       

