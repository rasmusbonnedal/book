# Data flows for verifikat

## Init

1. Grundbok selection changes

2. Verifikat view is cleared

3. For each row in verifikat

   * Add row to view `(id, konto, pengar)`

## Update

1. When row is updated in view, signal_edited emits `(id, konto', pengar')`

## Add row

1. x
