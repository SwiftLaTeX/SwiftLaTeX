import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import { isImageFile } from '../../utils/fileUtilities';
import { FileManagerEntry } from '../../types';

type Props = {
    entry: FileManagerEntry;
    entryPoint: string;
};

export default function FileListEntryIcon({ entry, entryPoint }: Props) {
    let icon;

    if (entry.item.path === entryPoint) {
        icon = (
            <g>
                <path d="M7.46487122,14 C7.80521143,13.4116533 8,12.7285743 8,12 C8,9.790861 6.209139,8 4,8 C3.65470043,8 3.31961992,8.043753 3,8.12601749 L3,2 L13,2 L13,14 L7.46487122,14 Z M9,2 L13,6 L13,2 L9,2 Z M9,6 L9,2 L8,2 L8,7 L13,7 L13,6 L9,6 Z" />
                <circle cx="4" cy="12" r="3" />
            </g>
        );
    } else if (entry.item.type === 'folder') {
        if (entry.state.isExpanded) {
            icon = (
                <g>
                    <path d="M7.5,5 L2,5 L2,13 L7.75,13 L14,13 L14,4 L15,4 L15,14 L1,14 L1,4 L6.5,4 L5.5,5 L7.5,5 L7.5,4.5 L7.5,5 Z M14,4 L14,3 L7.5,3 L7.5,3.5 L7.5,3 L7,3.5 L7,2 L15,2 L15,4 L14,4 Z M6.5,4 L5,4 L7,2 L7,3.5 L6.5,4 Z" />
                    <polygon points="0 7 13 7 14 14 1 14" />
                </g>
            );
        } else {
            icon = (
                <path d="M7.25,4 L7.5,4 L7.5,3 L7,3.5 L7,2 L15,2 L15,4 L7.25,4 Z M6.75,4 L5,4 L7,2 L7,3.5 L6.5,4 L6.75,4 Z M1,4 L15,4 L15,14 L1,14 L1,4 Z M7.5,3 L7.5,4 L14,4 L14,3 L7.5,3 Z" />
            );
        }
    } else if (isImageFile(entry.item.path)) {
        icon = (
            <g>
                <circle cx="5" cy="5" r="1" />
                <polygon points="5.71428571 8.41176471 8 11.2352941 10.8571429 7 14 13 2 13" />
                <path d="M3,3 L3,13 L13,13 L13,3 L3,3 Z M3,2 L13,2 C13.5522847,2 14,2.44771525 14,3 L14,13 C14,13.5522847 13.5522847,14 13,14 L3,14 C2.44771525,14 2,13.5522847 2,13 L2,3 C2,2.44771525 2.44771525,2 3,2 Z" />
            </g>
        );
    } else {
        icon = (
            <path d="M3,2 L13,2 L13,14 L3,14 L3,2 Z M9,2 L13,6 L13,2 L9,2 Z M9,6 L9,2 L8,2 L8,7 L13,7 L13,6 L9,6 Z" />
        );
    }

    return (
        <svg className={css(styles.icon)} viewBox="0 0 16 16">
            {icon}
        </svg>
    );
}

const styles = StyleSheet.create({
    icon: {
        height: 16,
        width: 16,
        fill: 'currentColor',
        verticalAlign: 'middle',
        opacity: 0.7,
    },
});
