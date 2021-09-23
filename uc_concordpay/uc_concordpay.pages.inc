<?php
/**
 * @file
 * Page callbacks for ConcordPay.
 */

/**
 * Callback function return page.
 *
 * @throws \Exception
 */
function uc_concordpay_page_callback() {
    $data = json_decode(file_get_contents("php://input"), TRUE);
    if (!isset($data['transactionStatus'], $data['type']) || empty($data)) {
        drupal_goto('<front>');
        die();
    }

    $cart_id = '';
    if (isset($_GET['uc_cart_id'])) {
        $cart_id = htmlspecialchars($_GET['uc_cart_id']);
    }

    $concordpay = new Concordpay();
    [$orderId, ] = explode(concordpay::ORDER_SEPARATOR, $data['orderReference']);
    $order = uc_order_load($orderId);
    $paymentInfo = $concordpay->isPaymentValid($data);

    if ($data['transactionStatus'] === concordpay::ORDER_DECLINED) {
        uc_order_update_status(
            $order->order_id,
            variable_get('cp_decline_order_status', concordpay::ORDER_STATUS_DECLINED)
        );
    }

    if (is_string($paymentInfo)) {
        die($paymentInfo);
    }

    if (!isset($data['type']) || !in_array($data['type'], $concordpay->getOperationTypes(), TRUE)) {
        die(t('Error: Unknown operation type'));
    }

    if ($paymentInfo === TRUE && $data['transactionStatus'] === concordpay::ORDER_APPROVED) {
        if ($data['type'] === $concordpay::RESPONSE_TYPE_PAYMENT) {
            $approve_status = variable_get('cp_approve_order_status', concordpay::ORDER_STATUS_COMPLETED);
            if ($order->order_status === $approve_status) {
                die(t('Error: This order has already been paid'));
            }
            // Ordinary payment.
            uc_order_update_status(
                $order->order_id,
                $approve_status
            );
            // Admin comment (table: uc_order_admin_comments).
            uc_order_comment_save($order->order_id, 0, t('Payment made via ConcordPay'));
            watchdog('uc_concordpay', 'Order #@order paid successfully.', ['@order' => $order->order_id], WATCHDOG_INFO);
            uc_cart_empty($cart_id);
            die('Ok');
        }

        if ($data['type'] === $concordpay::RESPONSE_TYPE_REVERSE) {
            // Refunded payment.
            uc_order_update_status(
                $order->order_id,
                variable_get('cp_refunded_order_status', concordpay::ORDER_STATUS_CANCELED)
            );
            // Admin comment (table: uc_order_admin_comments).
            uc_order_comment_save($order->order_id, 0, t('Payment have been refunded through ConcordPay'));
            watchdog('uc_concordpay', 'Payment for order #@order refund successfully.', ['@order' => $order->order_id], WATCHDOG_INFO);
            die(t('Payment refunded'));
        }
    }

    uc_order_update_status($order->order_id, concordpay::ORDER_STATUS_DECLINED);
    die(t('Payment declined'));
}

/**
 * Payer will be redirected to this page after success payment.
 */
function uc_concordpay_success() {
    $build = [];
    $message_alert = t('Thank you, payment approved.');
    drupal_set_message($message_alert, 'status');
    $build['message_success']['#markup'] = t('Payment processed successfully.') . '<br>';
    $build['message_info']['#markup'] = t('You can find a list of your orders in your personal account.');

    return $build;
}

/**
 * Payer will be redirected to this page after unsuccessful payment.
 */
function uc_concordpay_fail() {
    $build = [];
    $message_alert = t('Unfortunately payment declined.');
    drupal_set_message($message_alert, 'error');
    $build['message_fail']['#markup'] = t('Payment processed fail.') . '<br>';
    $build['message_info']['#markup'] = t('You can find a list of your orders in your personal account.');

    return $build;
}

/**
 * The buyer will be redirected to this page if the payment is canceled.
 */
function uc_concordpay_cancel() {
    $build = [];
    $message_alert = t('Canceled payment.');
    drupal_set_message($message_alert, 'warning');
    $build['message_cancel']['#markup'] = t('Cancellation of payment at the request of the buyer.') . '<br>';
    $build['message_info']['#markup'] = t('You can find a list of your orders in your personal account.');

    return $build;
}