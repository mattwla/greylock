#include "importpage.hpp"

#include "mainwizard.hpp"

Wizard::ImportPage::ImportPage(QWidget *parent) :
    QWizardPage(parent)
{
    mWizard = qobject_cast<MainWizard*>(parent);

    setupUi(this);

    registerField(QLatin1String("installation.import-settings"), importCheckBox);
    registerField(QLatin1String("installation.import-addons"), addonsCheckBox);

	//this->setVisible(false);
	importCheckBox->setVisible(false);
	addonsCheckBox->setVisible(false);
}

int Wizard::ImportPage::nextId() const
{
    return MainWizard::Page_Conclusion;
}
