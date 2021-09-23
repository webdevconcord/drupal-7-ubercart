# Модуль ConcordPay для Drupal Ubercart

Для работы модуля у вас должны быть установлены CMS **Drupal 7.7+** и плагин **Ubercart 3+**.

## Установка

1. Распакуйте архив с плагином и поместите файлы в каталог *{YOUR_SITE}/sites/all/modules/ubercart/payment/*.

2. Перейдите в *«Панель администратора -> Модули(admin/modules)»*, выберите модуль **ConcordPay Payment Gateway** и нажмите кнопку *«Сохранить»*.

3. Перейдите в настройки платежных сервисов вашего магазина
(*«Home -> Administration -> Store -> Configuration -> Payment method», admin/store/settings/payment*).

4. Нажмите на ссылку *Settings* напротив модуля **ConcordPay**.

5. Введите данные вашего продавца, полученные от платёжной системы, и сохраните настройки.
    - *Идентификатор продавца (Merchant ID)*;
    - *Секретный ключ (Secret Key)*.

6. Включите модуль оплаты **ConcordPay**.

*Модуль протестирован для работы с Drupal 7.82, Ubercart 3.13 и PHP 7.2*.