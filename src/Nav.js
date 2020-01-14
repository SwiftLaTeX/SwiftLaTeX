import React from 'react'
import { Dropdown, Menu } from 'semantic-ui-react'


const NavMenu = () => (
    <div>
        <Menu attached='top'>
            <Dropdown item icon='wrench' simple text="Files">
                    <Dropdown.Menu>
                        <Dropdown.Item text='New' />
                        <Dropdown.Item text='Open...' description='ctrl + o' />
                        <Dropdown.Item text='Save as...' description='ctrl + s' />
                        <Dropdown.Item text='Rename' description='ctrl + r' />
                        <Dropdown.Item text='Make a copy' />
                        <Dropdown.Item icon='folder' text='Move to folder' />
                        <Dropdown.Item icon='trash' text='Move to trash' />
                        <Dropdown.Divider />
                        <Dropdown.Item text='Download As...' />
                        <Dropdown.Item text='Publish To Web' />
                        <Dropdown.Item text='E-mail Collaborators' />
                    </Dropdown.Menu>
            </Dropdown>
        </Menu>
    </div>
);

export default NavMenu
