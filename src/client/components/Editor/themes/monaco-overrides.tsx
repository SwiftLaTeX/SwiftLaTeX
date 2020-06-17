import colors from '../../../configs/colors';
import { c } from '../../ColorsProvider';

const css = String.raw;

export default css`
  /* Common overrides */
  .snack-monaco-editor .monaco-editor .line-numbers {
    color: currentColor;
    opacity: 0.5;
  }

  /* Context menu overrides */
  .snack-monaco-editor .context-view.monaco-menu-container {
    font-family: 'Source Sans Pro', 'Helvetica Neue', Helvetica, Arial, sans-serif;
    background-color: transparent;
    box-shadow: none;
    border: none;
  }

  .snack-monaco-editor .monaco-menu .monaco-action-bar {
    padding: 4px;
    border-radius: 3px;
    border-style: solid;
    box-shadow: 0px 2px 6px rgba(0, 0, 0, 0.16) !important;
    background-color: ${c('content')} !important;
    color: ${c('text')} !important;
    border-color: ${c('editor-border')} !important;
  }

  .snack-monaco-editor.theme-light .monaco-menu .monaco-action-bar {
    border-width: 0;
  }

  .snack-monaco-editor.theme-dark .monaco-menu .monaco-action-bar {
    border-width: 1px;
  }

  .snack-monaco-editor .monaco-menu .monaco-action-bar .action-item .action-label,
  .snack-monaco-editor .monaco-menu .monaco-action-bar .action-item .action-label:focus {
    font-size: 14px;
    line-height: 1;
    color: inherit;
    border: 0;
  }

  .snack-monaco-editor .monaco-menu .monaco-action-bar .action-item .action-menu-item,
  .snack-monaco-editor .monaco-menu .monaco-action-bar .action-item .action-menu-item:focus {
    color: inherit !important;
    text-shadow: none !important;
  }

  .snack-monaco-editor .monaco-menu .monaco-action-bar .action-item.disabled .action-menu-item {
    pointer-events: none;
  }

  .snack-monaco-editor .monaco-menu .monaco-action-bar .action-item.focused:not(.disabled),
  .snack-monaco-editor .monaco-menu .monaco-action-bar .action-item:hover:not(.disabled) {
    border-radius: 2px;
    background-color: ${colors.primary} !important;
    color: white !important;
  }

  .snack-monaco-editor .monaco-menu .monaco-action-bar .action-item .action-menu-item {
    background-color: transparent !important;
    color: inherit !important;
  }

  .snack-monaco-editor
    .monaco-menu
    .monaco-action-bar
    .action-item
    .action-menu-item:focus:not(.disabled)
    .action-label,
  .snack-monaco-editor
    .monaco-menu
    .monaco-action-bar
    .action-item:hover:not(.disabled)
    .action-label {
    color: inherit;
  }

  .snack-monaco-editor .monaco-menu .monaco-action-bar .keybinding {
    color: inherit;
    font-size: inherit;
    opacity: 0.3;
  }

  .snack-monaco-editor .monaco-menu .monaco-action-bar .keybinding,
  .snack-monaco-editor .monaco-menu .monaco-action-bar .action-label:not(.separator) {
    padding: 8px 12px;
  }

  .snack-monaco-editor .monaco-action-bar .action-label.separator {
    border-bottom-color: currentColor;
    opacity: 0.1;
  }
`;
