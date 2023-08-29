#ifndef FORM_COLLAPSED_WIDGET_H
#define FORM_COLLAPSED_WIDGET_H

#include <QWidget>

/*!
    \class  FormCollapsedWidget

    \brief  The FormCollapsedWidget class implements a button to expand area.
            This widget is using a grid layout and places the items
            in the following way:
    \code
            +-------------+-------------------------+----------------+
            +    title                              |         Button |
            +-------------+-------------------------+----------------+
            |                   widget                               |
            +-------------+-------------------------+----------------+
    \endcode
*/

class FormCollapsedWidgetData;

class FormCollapsedWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FormCollapsedWidget(QWidget *parent = 0);
    virtual ~FormCollapsedWidget();

    enum WidgetState
    {
        E_EXPANDED,
        E_COLLAPSED,
        E_TITLE_NONE,
        E_TITLE_ONLY,
        E_BUTTON_NONE
    };

    WidgetState State() const;
    QString Title() const;
    QWidget *Widget() const;
    QString ButtonText() const;
    QFont TitleFont() const;
    bool IsChecked() const;
    bool IsCheckBoxValid() const;

    void SetState(WidgetState state);
    void SetTitle(const QString &text);
    void SetWidget(QWidget *widget);
    void SetButtonText(const QString &text);
    void SetTitleFont(const QFont &font);
    void SetChecked(bool checked);
    void SetCheckBoxValid(bool is_valid);
    void SetIcon(const QIcon &icon);
    void SetExpanded(bool expanded);
    QWidget *TakeWidget();

    static QPixmap CreateBackground(const QSize &size, QWidget *widget);

protected:
    void paintEvent(QPaintEvent *paint_event) override;

private:
    FormCollapsedWidgetData *data_;

signals:
    void SigChecked(bool checked);
    void SigLinkActivated(const QString &link);
    void SigExpanded(bool expanded);
};

#endif  // FORM_COLLAPSED_WIDGET_H
