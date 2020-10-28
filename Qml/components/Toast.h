#pragma once
#include <QQuickItem>
namespace Hix
{
	namespace QML
	{
		class ToastShell : public QQuickItem
		{
			Q_OBJECT
			Q_PROPERTY(QString message MEMBER _message NOTIFY messageChanged)
			Q_PROPERTY(MessageType messageType MEMBER _msgType NOTIFY messageTypeChanged)
			Q_ENUMS(MessageType)

		public:	
			ToastShell(QQuickItem* parent = nullptr);
			virtual ~ToastShell();

			enum class MessageType { Done, ExpectedError, UnExpectedError };
			void setMessage(MessageType type, std::string text);

		signals:
			void messageChanged();
			void messageTypeChanged();

		//private:
			//QQuickMouseArea* _mouseArea;

		private:
			QString _message = "Support generation completed";
			MessageType _msgType = MessageType::Done;
		};
		
	}
}
